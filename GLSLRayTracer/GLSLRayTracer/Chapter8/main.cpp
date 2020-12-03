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

class Chapter8 : public FrameWork
{
public:
	Chapter8()
	{
	}

	virtual ~Chapter8()
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
		pathTraceShaderProgram.SetUniform1i("world.objects[0].materialType", MAT_LAMBERTIAN);
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

		vertexArrayObject.Bind();
		vertexArrayObject.Draw(GL_TRIANGLES, 6);

		//////////////////////////////////////////////////////
		// draw to frame buffer
		frameBufferTexture.UnBindFrameBuffer();
		frameBufferTexture.Bind(0);
		proprocessingShaderProgram.SetUniform2f("screenSize", SCR_WIDTH, SCR_HEIGHT);
		proprocessingShaderProgram.SetUniform1i("frameBufferTexture", 0);
		vertexArrayObject.Bind();
		vertexArrayObject.Draw(GL_TRIANGLES, 6);

		return true;
	}

	void OnDestroy() override
	{
		frameBufferTexture.Destroy();

		vertexArrayObject.Destroy();

		envMap.Destroy();
		pathTraceShaderProgram.Destroy();

		proprocessingShaderProgram.Destroy();
	}
private:
	FrameBufferTexture2D frameBufferTexture;

	VertexArrayObject vertexArrayObject;

	TextureCube envMap;
	ShaderProgram pathTraceShaderProgram;

	ShaderProgram proprocessingShaderProgram;

	int sampleCount;

	vec3 cameraPos;
	vec3 cameraTarget;
	vec3 cameraUp;
};

int main()
{
	Chapter8 chapter;

	if (!chapter.Create(SCR_WIDTH, SCR_HEIGHT))
		return -1;

	chapter.Start();

	chapter.Destroy();

	return 0;
}