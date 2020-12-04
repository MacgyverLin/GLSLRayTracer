#include "FrameWork.h"
#include "ShaderProgram.h"
#include "Texture.h"
#include "VertexArrayObject.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"

#define SCR_WIDTH 800
#define SCR_HEIGHT 400

#define MAT_LAMBERTIAN 0
#define MAT_METALLIC 1
#define MAT_DIELECTRIC 2
#define MAT_PBR 3

class Chapter13 : public FrameWork
{
public:
	Chapter13()
	{
	}

	virtual ~Chapter13()
	{
	}

	virtual bool OnCreate() override
	{
		cameraPos = vec3(0.0f, 0.0f, 0.0f);
		cameraTarget = vec3(0.0f, 0.0f, -1.0f);
		cameraUp = vec3(0.0f, 1.0f, 0.0f);

		float vertices[] = {
			 1.0f,  1.0f, 0.0f,  // top right
			 1.0f, -1.0f, 0.0f,  // bottom right
			-1.0f, -1.0f, 0.0f,  // bottom left
			-1.0f,  1.0f, 0.0f   // top left 
		};
		unsigned int indices[] = {  // note that we start from 0!
			0, 1, 3,   // first triangle
			1, 2, 3    // second triangle
		};

		if (!vertexArrayObject.Create(vertices, sizeof(vertices) / sizeof(vertices[0]), indices, sizeof(indices) / sizeof(indices[0])))
		{
			return false;
		}

		if (!envMap.Create("../assets/env1.png"))
		{
			return false;
		}

		if (!pathTraceShaderProgram.Create("PathTraceVS.glsl", "PathTracePS.glsl"))
		{
			return false;
		}

		if (!frameBufferTexture.Create(SCR_WIDTH, SCR_HEIGHT, 4, true))
		{
			return false;
		}

		for (int i = 0; i < 2; i++)
		{
			if (!pingpongTexture[i].Create(SCR_WIDTH, SCR_HEIGHT, 4, true))
			{
				return false;
			}
		}		

		if (!proprocessingShaderProgram.Create("BlitVS.glsl", "BlitPS.glsl"))
		{
			return false;
		}

		sampleCount = 10;

		return true;
	}

	virtual bool OnUpdate() override
	{
		float theta;
		float phi;
		GetTheta(theta, phi);

		float y = sin(phi * 3.14 / 180.0f);
		float x = cos(phi * 3.14 / 180.0f) * cos(theta * 3.14 / 180.0f);
		float z = cos(phi * 3.14 / 180.0f) * sin(theta * 3.14 / 180.0f);

		cameraTarget[0] = cameraPos[0] + x;
		cameraTarget[1] = cameraPos[1] + y;
		cameraTarget[2] = cameraPos[2] + z;
		if (IsKeyPressed('W'))
		{
			cameraPos += (cameraTarget - cameraPos) * 0.016;
		}
		if (IsKeyPressed('S'))
		{
			cameraPos -= (cameraTarget - cameraPos) * 0.016;
		}
		if (IsKeyPressed('D'))
		{
			cameraPos += (cameraTarget - cameraPos).Cross(vec3(0, 1, 0)) * 0.016;
		}
		if (IsKeyPressed('A'))
		{
			cameraPos -= (cameraTarget - cameraPos).Cross(vec3(0, 1, 0)) * 0.016;
		}

		static int dir = 1;
		if (IsKeyPressed(' '))
		{
			sampleCount += dir;
			if (sampleCount > 100)
			{
				sampleCount = 100;
				dir = -1;
			}
			if (sampleCount < 0)
			{
				sampleCount = 0;
				dir = +1;
			}

			printf("%d\n", sampleCount);
		}

		static int blur = 5;
		static int blurdir = 1;
		if (IsKeyPressed('B'))
		{
			blur += blurdir;
			if (blur > 300)
			{
				blur = 300;
				blurdir = -1;
			}
			if (blur < 0)
			{
				blur = 0;
				blurdir = +1;
			}

			printf("%d\n", blur);
		}

		static float threshold = 0.95;
		static float thresholddir = 1;
		if (IsKeyPressed('T'))
		{
			threshold += thresholddir * 0.01;
			if (threshold > 1)
			{
				threshold = 1;
				thresholddir = -1;
			}
			if (threshold < 0)
			{
				threshold = 0;
				thresholddir = 1;
			}

			printf("%f\n", threshold);
		}


		static float metallic = 1.0;
		if (IsKeyPressed('H'))
		{
			metallic += 0.01;
			if (metallic > 1)
				metallic = 1;
			printf("metallic=%f\n", metallic);
		}
		if (IsKeyPressed('N'))
		{
			metallic -= 0.01;
			if (metallic < 0)
			{
				metallic = 0;
			}
			printf("metallic=%f\n", metallic);
		}

		static float roughness = 0.0;
		if (IsKeyPressed('J'))
		{
			roughness += 0.01;
			if (roughness > 1)
				roughness = 1;
			printf("roughness=%f\n", roughness);
		}
		if (IsKeyPressed('M'))
		{
			roughness -= 0.01;
			if (roughness < 0)
			{
				roughness = 0;
			}
			printf("roughness=%f\n", roughness);
		}


		//////////////////////////////////////////////////////
		glClearColor(0.0f, 0.5f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		frameBufferTexture.BindFrameBuffer();
		envMap.Bind(0);

		pathTraceShaderProgram.Bind();
		pathTraceShaderProgram.SetUniform2f("screenSize", SCR_WIDTH, SCR_HEIGHT);
		pathTraceShaderProgram.SetUniform1i("envMap", 0);
		pathTraceShaderProgram.SetUniform1f("envMapIntensity", 1.0);
		pathTraceShaderProgram.SetUniform1i("sampleCount", sampleCount);

		pathTraceShaderProgram.SetUniform3f("camera.origin", cameraPos[0], cameraPos[1], cameraPos[2]);
		pathTraceShaderProgram.SetUniform3f("camera.target", cameraTarget[0], cameraTarget[1], cameraTarget[2]);
		pathTraceShaderProgram.SetUniform3f("camera.up", 0, 1, 0);
		pathTraceShaderProgram.SetUniform1f("camera.vfov", 90.0f);
		pathTraceShaderProgram.SetUniform1f("camera.aspect", float(SCR_WIDTH) / SCR_HEIGHT);

		pathTraceShaderProgram.SetUniform1i("world.objectCount", 4);
		pathTraceShaderProgram.SetUniform3f("world.objects[0].center", 0.0, 0.0, -1.0);
		pathTraceShaderProgram.SetUniform1f("world.objects[0].radius", 0.5);
		pathTraceShaderProgram.SetUniform1i("world.objects[0].materialType", MAT_PBR);
		pathTraceShaderProgram.SetUniform1i("world.objects[0].material", 0);
		pathTraceShaderProgram.SetUniform3f("world.objects[1].center", 0.0, -100.5, -1.0);
		pathTraceShaderProgram.SetUniform1f("world.objects[1].radius", 100.0);
		pathTraceShaderProgram.SetUniform1i("world.objects[1].materialType", MAT_LAMBERTIAN);
		pathTraceShaderProgram.SetUniform1i("world.objects[1].material", 1);
		pathTraceShaderProgram.SetUniform3f("world.objects[2].center", 1.0, 0.0, -1.0);
		pathTraceShaderProgram.SetUniform1f("world.objects[2].radius", 0.5);
		pathTraceShaderProgram.SetUniform1i("world.objects[2].materialType", MAT_METALLIC);
		pathTraceShaderProgram.SetUniform1i("world.objects[2].material", 0);
		pathTraceShaderProgram.SetUniform3f("world.objects[3].center", -1.0, 0.0, -1.0);
		pathTraceShaderProgram.SetUniform1f("world.objects[3].radius", 0.5);
		pathTraceShaderProgram.SetUniform1i("world.objects[3].materialType", MAT_DIELECTRIC);
		pathTraceShaderProgram.SetUniform1i("world.objects[3].material", 0);

		pathTraceShaderProgram.SetUniform3f("lambertMaterials[0].albedo", 0.1, 0.2, 0.5);
		pathTraceShaderProgram.SetUniform3f("lambertMaterials[1].albedo", 0.8, 0.8, 0.0);
		pathTraceShaderProgram.SetUniform3f("lambertMaterials[2].albedo", 0.0, 1.0, 0.0);
		pathTraceShaderProgram.SetUniform3f("lambertMaterials[3].albedo", 0.0, 0.0, 1.0);

		pathTraceShaderProgram.SetUniform3f("metallicMaterials[0].albedo", 0.8, 0.6, 0.2);
		pathTraceShaderProgram.SetUniform1f("metallicMaterials[0].roughness", 0.0);
		pathTraceShaderProgram.SetUniform3f("metallicMaterials[1].albedo", 0.8, 0.6, 0.0);
		pathTraceShaderProgram.SetUniform1f("metallicMaterials[1].roughness", 0.0);
		pathTraceShaderProgram.SetUniform3f("metallicMaterials[2].albedo", 0.0, 0.0, 1.0);
		pathTraceShaderProgram.SetUniform1f("metallicMaterials[2].roughness", 0.0);
		pathTraceShaderProgram.SetUniform3f("metallicMaterials[3].albedo", 0.0, 0.0, 1.0);
		pathTraceShaderProgram.SetUniform1f("metallicMaterials[3].roughness", 0.0);

		pathTraceShaderProgram.SetUniform3f("dielectricMaterials[0].albedo", 1.0, 1.0, 1.0);
		pathTraceShaderProgram.SetUniform1f("dielectricMaterials[0].roughness", 0.0);
		pathTraceShaderProgram.SetUniform1f("dielectricMaterials[0].ior", 1.5);
		pathTraceShaderProgram.SetUniform3f("dielectricMaterials[1].albedo", 1.0, 1.0, 1.0);
		pathTraceShaderProgram.SetUniform1f("dielectricMaterials[1].roughness", 0.0);
		pathTraceShaderProgram.SetUniform1f("dielectricMaterials[1].ior", 1.5);
		pathTraceShaderProgram.SetUniform3f("dielectricMaterials[2].albedo", 1.0, 1.0, 1.0);
		pathTraceShaderProgram.SetUniform1f("dielectricMaterials[2].roughness", 0.0);
		pathTraceShaderProgram.SetUniform1f("dielectricMaterials[2].ior", 1.5);
		pathTraceShaderProgram.SetUniform3f("dielectricMaterials[3].albedo", 1.0, 1.0, 1.0);
		pathTraceShaderProgram.SetUniform1f("dielectricMaterials[3].roughness", 0.0);
		pathTraceShaderProgram.SetUniform1f("dielectricMaterials[3].ior", 1.5);

#define WATER 0.02, 0.02, 0.02
#define PLASTIC_LOW 0.03, 0.03, 0.03
#define PLASTIC_HIGH 0.05, 0.05, 0.05
#define GLASS_HIGH 0.02, 0.02, 0.02
#define DIAMOND 0.17, 0.17, 0.17
#define IRON 0.56, 0.57, 0.58
#define COPPER 0.95, 0.64, 0.54
#define GOLD 1.00, 0.71, 0.29
#define ALUMINIUM 0.91, 0.92, 0.92
#define SILVER 0.96, 0.93, 0.88

		pathTraceShaderProgram.SetUniform3f("pbrMaterials[0].albedo", WATER);
		pathTraceShaderProgram.SetUniform1f("pbrMaterials[0].metallic", metallic);
		pathTraceShaderProgram.SetUniform1f("pbrMaterials[0].roughness", roughness);

		vertexArrayObject.Bind();
		vertexArrayObject.Draw(GL_TRIANGLES, 6);

		//////////////////////////////////////////////////////
		int last = 0;
		for (int i = 0; i < blur; i++)
		{
			int ping = (i%2);
			int pong = (i+1)%2;

			pingpongTexture[pong].BindFrameBuffer();
			if(i==0)
				frameBufferTexture.Bind(0);
			else
				pingpongTexture[ping].Bind(0);

			pingpongTexture[ping].SetWarpS(GL_CLAMP);
			pingpongTexture[ping].SetWarpT(GL_CLAMP);
			pingpongTexture[ping].SetWarpR(GL_CLAMP);

			proprocessingShaderProgram.Bind();
			proprocessingShaderProgram.SetUniform2f("screenSize", SCR_WIDTH, SCR_HEIGHT);
			proprocessingShaderProgram.SetUniform1i("frameBufferTexture", 0);
			proprocessingShaderProgram.SetUniform1i("horizontal", ping);
			proprocessingShaderProgram.SetUniform1i("useGussian", i!=0);
			proprocessingShaderProgram.SetUniform1i("useThreshold", i==0);
			proprocessingShaderProgram.SetUniform1i("useBlend", false);
			proprocessingShaderProgram.SetUniform1f("threshold", threshold);

			vertexArrayObject.Bind();
			vertexArrayObject.Draw(GL_TRIANGLES, 6);

			last = pong;
		}
		
		// draw to frame buffer
		pingpongTexture[last].UnBindFrameBuffer();
		frameBufferTexture.Bind(0);
		proprocessingShaderProgram.SetUniform2f("screenSize", SCR_WIDTH, SCR_HEIGHT);
		proprocessingShaderProgram.SetUniform1i("frameBufferTexture", 0);
		proprocessingShaderProgram.SetUniform1i("horizontal", 0);
		proprocessingShaderProgram.SetUniform1i("useGussian", false);
		proprocessingShaderProgram.SetUniform1i("useThreshold", false);
		proprocessingShaderProgram.SetUniform1i("useBlend", false);
		proprocessingShaderProgram.SetUniform1f("threshold", threshold);
		vertexArrayObject.Bind();
		vertexArrayObject.Draw(GL_TRIANGLES, 6);
		/*
		glEnable(GL_BLEND);
		glBlendEquation(GL_ADD);
		glBlendFunc(GL_ONE, GL_ONE);
		// draw to frame buffer
		pingpongTexture[last].UnBindFrameBuffer();
		pingpongTexture[last].Bind(0);
		proprocessingShaderProgram.SetUniform2f("screenSize", SCR_WIDTH, SCR_HEIGHT);
		proprocessingShaderProgram.SetUniform1i("frameBufferTexture", 0);
		proprocessingShaderProgram.SetUniform1i("horizontal", 0);
		proprocessingShaderProgram.SetUniform1i("useGussian", false);
		proprocessingShaderProgram.SetUniform1i("useThreshold", false);
		proprocessingShaderProgram.SetUniform1i("useBlend", true);
		proprocessingShaderProgram.SetUniform1f("threshold", threshold);
		vertexArrayObject.Bind();
		vertexArrayObject.Draw(GL_TRIANGLES, 6);

		glDisable(GL_BLEND);
		*/

		return true;
	}

	void OnDestroy() override
	{
		vertexArrayObject.Destroy();

		envMap.Destroy();
		pathTraceShaderProgram.Destroy();

		for(int i=0; i<2; i++)
			pingpongTexture[i].Destroy();
		
		proprocessingShaderProgram.Destroy();
	}
private:
	VertexArrayObject vertexArrayObject;

	TextureCube envMap;
	ShaderProgram pathTraceShaderProgram;

	FrameBufferTexture2D pingpongTexture[2];
	ShaderProgram proprocessingShaderProgram;

	FrameBufferTexture2D frameBufferTexture;

	int sampleCount;

	vec3 cameraPos;
	vec3 cameraTarget;
	vec3 cameraUp;
};

int main()
{
	Chapter13 chapter;

	if (!chapter.Create(SCR_WIDTH, SCR_HEIGHT))
		return -1;

	chapter.Start();

	chapter.Destroy();

	return 0;
}