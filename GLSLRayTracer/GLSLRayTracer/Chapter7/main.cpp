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

class Chapter7 : public FrameWork
{
public:
	Chapter7()
	{
	}

	virtual ~Chapter7()
	{
	}

	virtual bool OnCreate() override
	{
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

		if (!shaderProgram.Create("PathTraceVS.glsl", "PathTracePS.glsl"))
		{
			return false;
		}

		if (!envMap.Create("../assets/photo_studio_01_1k.hdr"))
		{
			return false;
		}

		return true;
	}

	virtual bool OnUpdate() override
	{
		glClearColor(0.0f, 0.5f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		shaderProgram.Bind();
		shaderProgram.SetUniform2f("screenSize", SCR_WIDTH, SCR_HEIGHT);
		
		shaderProgram.SetUniform1i("envMap", 0);

		shaderProgram.SetUniform3f("camera.lower_left_corner", -2.0, -1.0, -1.0);
		shaderProgram.SetUniform3f("camera.horizontal", 4.0, 0.0, 0.0);
		shaderProgram.SetUniform3f("camera.vertical", 0.0, 2.0, 0.0);
		shaderProgram.SetUniform3f("camera.origin", 0.0, 0.0, 0.0);

		shaderProgram.SetUniform1i("world.objectCount", 4);
		shaderProgram.SetUniform3f("world.objects[0].center", 0.0, 0.0, -1.0);
		shaderProgram.SetUniform1f("world.objects[0].radius", 0.5);
		shaderProgram.SetUniform1i("world.objects[0].materialType", MAT_LAMBERTIAN);
		shaderProgram.SetUniform1i("world.objects[0].material", 0);
		shaderProgram.SetUniform3f("world.objects[1].center", 0.0, -100.5, -1.0);
		shaderProgram.SetUniform1f("world.objects[1].radius", 100.0);
		shaderProgram.SetUniform1i("world.objects[1].materialType", MAT_LAMBERTIAN);
		shaderProgram.SetUniform1i("world.objects[1].material", 1);
		shaderProgram.SetUniform3f("world.objects[2].center", 1.0, 0.0, -1.0);
		shaderProgram.SetUniform1f("world.objects[2].radius", 0.5);
		shaderProgram.SetUniform1i("world.objects[2].materialType", MAT_METALLIC);
		shaderProgram.SetUniform1i("world.objects[2].material", 0);
		shaderProgram.SetUniform3f("world.objects[3].center", -1.0, 0.0, -1.0);
		shaderProgram.SetUniform1f("world.objects[3].radius", 0.5);
		shaderProgram.SetUniform1i("world.objects[3].materialType", MAT_DIELECTRIC);
		shaderProgram.SetUniform1i("world.objects[3].material", 0);

		shaderProgram.SetUniform3f("lambertMaterials[0].albedo", 0.1, 0.2, 0.5);
		shaderProgram.SetUniform3f("lambertMaterials[1].albedo", 0.8, 0.8, 0.0);
		shaderProgram.SetUniform3f("lambertMaterials[2].albedo", 0.0, 1.0, 0.0);
		shaderProgram.SetUniform3f("lambertMaterials[3].albedo", 0.0, 0.0, 1.0);

		shaderProgram.SetUniform3f("metallicMaterials[0].albedo", 0.8, 0.6, 0.2);
		shaderProgram.SetUniform1f("metallicMaterials[0].roughness", 0.0);
		shaderProgram.SetUniform3f("metallicMaterials[1].albedo", 0.8, 0.6, 0.0);
		shaderProgram.SetUniform1f("metallicMaterials[1].roughness", 0.0);
		shaderProgram.SetUniform3f("metallicMaterials[2].albedo", 0.0, 0.0, 1.0);
		shaderProgram.SetUniform1f("metallicMaterials[2].roughness", 0.0);
		shaderProgram.SetUniform3f("metallicMaterials[3].albedo", 0.0, 0.0, 1.0);
		shaderProgram.SetUniform1f("metallicMaterials[3].roughness", 0.0);

		shaderProgram.SetUniform3f("dielectricMaterials[0].albedo", 1.0, 1.0, 1.0);
		shaderProgram.SetUniform1f("dielectricMaterials[0].roughness", 0.0);
		shaderProgram.SetUniform1f("dielectricMaterials[0].ior", 1.5);
		shaderProgram.SetUniform3f("dielectricMaterials[1].albedo", 1.0, 1.0, 1.0);
		shaderProgram.SetUniform1f("dielectricMaterials[1].roughness", 0.0);
		shaderProgram.SetUniform1f("dielectricMaterials[1].ior", 1.5);
		shaderProgram.SetUniform3f("dielectricMaterials[2].albedo", 1.0, 1.0, 1.0);
		shaderProgram.SetUniform1f("dielectricMaterials[2].roughness", 0.0);
		shaderProgram.SetUniform1f("dielectricMaterials[2].ior", 1.5);
		shaderProgram.SetUniform3f("dielectricMaterials[3].albedo", 1.0, 1.0, 1.0);
		shaderProgram.SetUniform1f("dielectricMaterials[3].roughness", 0.0);
		shaderProgram.SetUniform1f("dielectricMaterials[3].ior", 1.5);

		vertexArrayObject.Bind();

		
		envMap.Bind(0);

		vertexArrayObject.Draw(GL_TRIANGLES, 6);

		return true;
	}

	void OnDestroy() override
	{
		

		envMap.Destroy();

		shaderProgram.Destroy();

		vertexArrayObject.Destroy();
	}
private:
	ShaderProgram shaderProgram;
	
	Texture2D envMap;
	VertexArrayObject vertexArrayObject;
};

int main()
{
	Chapter7 chapter7;
	if (!chapter7.Create(SCR_WIDTH, SCR_HEIGHT))
		return -1;

	chapter7.Start();

	chapter7.Destroy();

	return 0;
}