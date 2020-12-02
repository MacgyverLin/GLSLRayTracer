#include "FrameWork.h"
#include "ShaderProgram.h"
#include "Texture.h"
#include "VertexArrayObject.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"

#define SCR_WIDTH 800
#define SCR_HEIGHT 400

class Chapter6 : public FrameWork
{
public:
	Chapter6()
	{
	}

	virtual ~Chapter6()
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

		shaderProgram.SetUniform1i("world.objectCount", 2);
		shaderProgram.SetUniform3f("world.objects[0].center", 0.0, 0.0, -1.0);
		shaderProgram.SetUniform1f("world.objects[0].radius", 0.5);
		shaderProgram.SetUniform3f("world.objects[1].center", 0.0, -100.5, -1.0);
		shaderProgram.SetUniform1f("world.objects[1].radius", 100.0);

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
	Chapter6 chapter6;

	if (!chapter6.Create(SCR_WIDTH, SCR_HEIGHT))
		return -1;

	chapter6.Start();

	chapter6.Destroy();

	return 0;
}