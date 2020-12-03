#ifndef _VertexArrayObject_h_
#define _VertexArrayObject_h_

#include <glad\glad.h>
#include <GLFW\glfw3.h>

class VertexArrayObject
{
public:
	VertexArrayObject()
		: VAO(0)
		, VBO(0)
		, EBO(0)
	{
	}

	virtual ~VertexArrayObject()
	{
		Destroy();
	}

	bool Create(float* vertices, int verticesCount, unsigned int* indices, int indicesCount)
	{
		glGenVertexArrays(1, &VAO);
		if (VAO == 0)
		{
			return false;
		}

		glGenBuffers(1, &VBO);
		if (VBO == 0)
		{
			return false;
		}

		glGenBuffers(1, &EBO);
		if (EBO == 0)
		{
			return false;
		}

		glBindVertexArray(VAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, verticesCount * sizeof(float), vertices, GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesCount * sizeof(int), indices, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		glBindVertexArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		return true;
	}

	void Destroy()
	{
		if (VAO)
		{
			if (VBO)
			{
				glDeleteBuffers(1, &VBO);
				VBO = 0;
			}

			if (EBO)
			{
				glDeleteBuffers(1, &EBO);
				EBO = 0;
			}

			glDeleteVertexArrays(1, &VAO);
			VAO = 0;
		}
	}

	void Bind()
	{
		if (VAO)
		{
			glBindVertexArray(VAO);
		}
	}

	void Unbind()
	{
		glBindVertexArray(0);
	}

	void Draw(unsigned int type_, unsigned int count_)
	{
		glDrawElements(type_, count_, GL_UNSIGNED_INT, 0);
	}
private:
	unsigned int VAO;
	unsigned int VBO;
	unsigned int EBO;
};

#endif