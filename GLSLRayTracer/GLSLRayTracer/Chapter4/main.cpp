#include "FrameWork.h"
#include "ShaderProgram.h"
#include "Texture.h"
#include "VertexArrayObject.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"

#define SCR_WIDTH 800
#define SCR_HEIGHT 400

class Chapter4 : public FrameWork
{
public:
	Chapter4()
	{
	}

	virtual ~Chapter4()
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

		if (!randomMap.Create())
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
		shaderProgram.SetUniform1i("randomMap", 0);
		shaderProgram.SetUniform1i("envMap", 1);

		shaderProgram.SetUniform3f("camera.lower_left_corner", -2.0, -1.0, -1.0);
		shaderProgram.SetUniform3f("camera.horizontal", 4.0, 0.0, 0.0);
		shaderProgram.SetUniform3f("camera.vertical", 0.0, 2.0, 0.0);
		shaderProgram.SetUniform3f("camera.origin", 0.0, 0.0, 0.0);

		vertexArrayObject.Bind();

		randomMap.Bind(0);
		envMap.Bind(1);

		vertexArrayObject.Draw(GL_TRIANGLES, 6);

		return true;
	}

	void OnDestroy() override
	{
		randomMap.Destroy();

		envMap.Destroy();

		shaderProgram.Destroy();

		vertexArrayObject.Destroy();
	}
private:
	ShaderProgram shaderProgram;
	RandomTexture2D randomMap;
	Texture2D envMap;
	VertexArrayObject vertexArrayObject;
};

int main()
{
	Chapter4 chapter4;

	if (!chapter4.Create(SCR_WIDTH, SCR_HEIGHT))
		return -1;

	chapter4.Start();

	chapter4.Destroy();

	return 0;
}