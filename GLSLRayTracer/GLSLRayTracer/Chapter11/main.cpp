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

class Chapter11 : public FrameWork
{
public:
	Chapter11()
	{
	}

	virtual ~Chapter11()
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

		if (!frameBufferTexture.Create(SCR_WIDTH, SCR_HEIGHT, 4, true))
		{
			return false;
		}

		if (!envMap.Create("../assets/env1.png"))
		{
			return false;
		}
		envMap.SetMagFilter(GL_LINEAR);
		envMap.SetMinFilter(GL_LINEAR_MIPMAP_LINEAR);

		if (!pathTraceShaderProgram.Create("PathTraceVS.glsl", "PathTracePS.glsl"))
		{
			return false;
		}

		if (!proprocessingShaderProgram.Create("BlitVS.glsl", "BlitPS.glsl"))
		{
			return false;
		}

		if (!vertexArrayObject.Create(vertices, sizeof(vertices) / sizeof(vertices[0]), indices, sizeof(indices) / sizeof(indices[0])))
		{
			return false;
		}

		sampleCount = 10;

		roughness = 0.0001f;
		metallic = 1.0f;
		anisotropic = 0.0f;

		envMapIntensity = 1.0f;
		A = 1.0f;
		return true;
	}

	void dump()
	{
		printf("roughness=%3.3f, metallic=%3.3f, anisotropic=%3.3f\n", roughness, metallic, anisotropic);
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
		if (IsKeyPressed(' '))
		{
			sampleCount++;
			if (sampleCount > 1000)
				sampleCount = 1;

			printf("%d\n", sampleCount);
		}
		if (IsKeyPressed('I'))
		{
			envMapIntensity += 0.1f;
			if (envMapIntensity > 10.0)
				envMapIntensity = 0.1f;

			printf("%f\n", envMapIntensity);
		}	
		if (IsKeyPressed('K'))
		{
			A += 0.1f;
			if (A > 10.0)
				A = 0.1f;

			printf("%f\n", A);
		}


		static int delay = 3;
		if (delay-- == 0)
		{
			delay = 3;
			if (IsKeyPressed('R'))
			{
				roughness += 0.1f;
				if (roughness > 1)
					roughness = 1;

				dump();
			}
			if (IsKeyPressed('F'))
			{
				roughness -= 0.1f;
				if (roughness < 0)
					roughness = 0;

				dump();
			}

			if (IsKeyPressed('T'))
			{
				metallic += 0.1f;
				if (metallic > 1)
					metallic = 1;

				dump();
			}
			if (IsKeyPressed('G'))
			{
				metallic -= 0.1f;
				if (metallic < 0)
					metallic = 0;

				dump();
			}

			if (IsKeyPressed('Y'))
			{
				anisotropic += 0.02f;
				if (anisotropic > 1)
					anisotropic = 1;

				dump();
			}
			if (IsKeyPressed('H'))
			{
				anisotropic -= 0.02f;
				if (anisotropic < -1)
					anisotropic = -1;

				dump();
			}
		}


		//////////////////////////////////////////////////////
		glClearColor(0.0f, 0.5f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		frameBufferTexture.BindFrameBuffer();

		envMap.Bind(0);

		pathTraceShaderProgram.Bind();
		pathTraceShaderProgram.SetUniform2f("screenSize", SCR_WIDTH, SCR_HEIGHT);
		pathTraceShaderProgram.SetUniform1i("envMap", 0);
		pathTraceShaderProgram.SetUniform1f("envMapIntensity", envMapIntensity);
		pathTraceShaderProgram.SetUniform1i("sampleCount", sampleCount);

		pathTraceShaderProgram.SetUniform3f("camera.origin", cameraPos[0], cameraPos[1], cameraPos[2]);
		pathTraceShaderProgram.SetUniform3f("camera.target", cameraTarget[0], cameraTarget[1], cameraTarget[2]);
		pathTraceShaderProgram.SetUniform3f("camera.up", 0, 1, 0);
		pathTraceShaderProgram.SetUniform1f("camera.vfov", 90.0f);
		pathTraceShaderProgram.SetUniform1f("camera.aspect", float(SCR_WIDTH) / SCR_HEIGHT);
		pathTraceShaderProgram.SetUniform1f("camera.aperture", 0.2f);
		pathTraceShaderProgram.SetUniform1f("camera.focalDistance", 3.0);

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
		pathTraceShaderProgram.SetUniform1f("metallicMaterials[0].roughness", roughness);
		pathTraceShaderProgram.SetUniform3f("metallicMaterials[1].albedo", 0.8, 0.6, 0.0);
		pathTraceShaderProgram.SetUniform1f("metallicMaterials[1].roughness", 0.0);
		pathTraceShaderProgram.SetUniform3f("metallicMaterials[2].albedo", 0.0, 0.0, 1.0);
		pathTraceShaderProgram.SetUniform1f("metallicMaterials[2].roughness", 0.0);
		pathTraceShaderProgram.SetUniform3f("metallicMaterials[3].albedo", 0.0, 0.0, 1.0);
		pathTraceShaderProgram.SetUniform1f("metallicMaterials[3].roughness", 0.0);

		pathTraceShaderProgram.SetUniform3f("dielectricMaterials[0].albedo", 1.0, 1.0, 1.0);
		pathTraceShaderProgram.SetUniform1f("dielectricMaterials[0].roughness", roughness);
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
		frameBufferTexture.UnBindFrameBuffer();

		frameBufferTexture.Bind(0);
		proprocessingShaderProgram.Bind();
		proprocessingShaderProgram.SetUniform2f("screenSize", SCR_WIDTH, SCR_HEIGHT);
		proprocessingShaderProgram.SetUniform1i("frameBufferTexture", 0);
		proprocessingShaderProgram.SetUniform1f("A", A);

		vertexArrayObject.Bind();
		vertexArrayObject.Draw(GL_TRIANGLES, 6);

		return true;
	}

	void OnDestroy() override
	{
		envMap.Destroy();
		pathTraceShaderProgram.Destroy();

		frameBufferTexture.Destroy();
		proprocessingShaderProgram.Destroy();
		vertexArrayObject.Destroy();
	}
private:
	FrameBufferTexture2D frameBufferTexture;

	TextureCube envMap;
	ShaderProgram pathTraceShaderProgram;

	ShaderProgram proprocessingShaderProgram;
	VertexArrayObject vertexArrayObject;

	int sampleCount;

	vec3 cameraPos;
	vec3 cameraTarget;
	vec3 cameraUp;

	float roughness;
	float metallic;
	float anisotropic;

	float envMapIntensity;
	float A;
};

int main()
{
	Chapter11 chapter;

	if (!chapter.Create(SCR_WIDTH, SCR_HEIGHT))
		return -1;

	chapter.Start();

	chapter.Destroy();

	return 0;
}