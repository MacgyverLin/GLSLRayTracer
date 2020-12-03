#ifndef _ShaderProgram_h_
#define _ShaderProgram_h_

#include <glad\glad.h>
#include <GLFW\glfw3.h>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class ShaderProgram
{
public:
	ShaderProgram()
		: handle(0)
	{
	}

	~ShaderProgram()
	{
		Destroy();
	}

	bool Create(const char* vertexPath, const char* fragmentPath, const char* geometryPath = nullptr)
	{
		// 1. retrieve the vertex/fragment source code from filePath
		std::string vertexCode;
		std::string fragmentCode;
		std::string geometryCode;
		std::ifstream vShaderFile;
		std::ifstream fShaderFile;
		std::ifstream gShaderFile;
		// ensure ifstream objects can throw exceptions:
		vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		gShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		try
		{
			// open files
			vShaderFile.open(vertexPath);
			fShaderFile.open(fragmentPath);
			if(geometryPath)
				gShaderFile.open(geometryPath);
			std::stringstream vShaderStream, fShaderStream, gShaderStream;

			// read file's buffer contents into streams
			vShaderStream << vShaderFile.rdbuf();
			fShaderStream << fShaderFile.rdbuf();
			if (geometryPath)
				gShaderStream << gShaderFile.rdbuf();

			// close file handlers
			vShaderFile.close();
			fShaderFile.close();
			if (geometryPath)
				gShaderFile.close();

			// convert stream into string
			vertexCode = vShaderStream.str();
			fragmentCode = fShaderStream.str();
			if (geometryPath)
				geometryCode = gShaderStream.str();
		}
		catch (std::ifstream::failure&)
		{
			return false;
		}

		const char* vShaderCode = vertexCode.c_str();
		const char* fShaderCode = fragmentCode.c_str();
		const char* gShaderCode = geometryCode.c_str();
		if (geometryPath)
			gShaderCode = geometryCode.c_str();

		// 2. compile shaders
		unsigned int vertex, fragment, geometry;
		// vertex shader
		vertex = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertex, 1, &vShaderCode, NULL);
		glCompileShader(vertex);
		CheckCompileErrors(vertex, "VERTEX");

		// fragment Shader
		fragment = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragment, 1, &fShaderCode, NULL);
		glCompileShader(fragment);
		CheckCompileErrors(fragment, "FRAGMENT");

		if (geometryPath)
		{
			// fragment Shader
			geometry = glCreateShader(GL_GEOMETRY_SHADER);
			glShaderSource(geometry, 1, &gShaderCode, NULL);
			glCompileShader(geometry);
			CheckCompileErrors(geometry, "GEOMETRY");
		}

		// shader Program
		handle = glCreateProgram();
		glAttachShader(handle, vertex);
		glAttachShader(handle, fragment);
		if (geometryPath)
		{
			glAttachShader(handle, geometry);
		}

		glLinkProgram(handle);
		CheckCompileErrors(handle, "PROGRAM");
		glDeleteShader(vertex);
		glDeleteShader(fragment);
		if(geometryPath)
		{
			glDeleteShader(geometry);
		}

		return true;
	}

	void Destroy()
	{
		if (handle)
		{
			glDeleteProgram(handle);

			handle = 0;
		}
	}

	void Bind()
	{
		if (handle)
			glUseProgram(handle);
	}

	void Unbind()
	{
		glUseProgram(0);
	}

	void SetUniform1i(const char* name_, int v0_)
	{
		glUniform1i(glGetUniformLocation(handle, name_), v0_);
	}

	void SetUniform2i(const char* name_, int v0_, int v1_)
	{
		glUniform2i(glGetUniformLocation(handle, name_), v0_, v1_);
	}

	void SetUniform3i(const char* name_, int v0_, int v1_, int v2_)
	{
		glUniform3i(glGetUniformLocation(handle, name_), v0_, v1_, v2_);
	}

	void SetUniform4i(const char* name_, int v0_, int v1_, int v2_, int v3_)
	{
		glUniform4i(glGetUniformLocation(handle, name_), v0_, v1_, v2_, v3_);
	}

	void SetUniform1f(const char* name_, float v0_)
	{
		glUniform1f(glGetUniformLocation(handle, name_), v0_);
	}

	void SetUniform2f(const char* name_, float v0_, float v1_)
	{
		glUniform2f(glGetUniformLocation(handle, name_), v0_, v1_);
	}

	void SetUniform3f(const char* name_, float v0_, float v1_, float v2_)
	{
		glUniform3f(glGetUniformLocation(handle, name_), v0_, v1_, v2_);
	}

	void SetUniform4f(const char* name_, float v0_, float v1_, float v2_, float v3_)
	{
		glUniform4f(glGetUniformLocation(handle, name_), v0_, v1_, v2_, v3_);
	}

	void SetUniform1iv(const char* name_, int count_, const int* v_)
	{
		glUniform1iv(glGetUniformLocation(handle, name_), count_, v_);
	}

	void SetUniform2iv(const char* name_, int count_, const int* v_)
	{
		glUniform2iv(glGetUniformLocation(handle, name_), count_, v_);
	}

	void SetUniform3iv(const char* name_, int count_, const int* v_)
	{
		glUniform3iv(glGetUniformLocation(handle, name_), count_, v_);
	}

	void SetUniform4iv(const char* name_, int count_, const int* v_)
	{
		glUniform4iv(glGetUniformLocation(handle, name_), count_, v_);
	}

	void SetUniform1fv(const char* name_, int count_, const float* v_)
	{
		glUniform1fv(glGetUniformLocation(handle, name_), count_, v_);
	}

	void SetUniform2fv(const char* name_, int count_, const float* v_)
	{
		glUniform2fv(glGetUniformLocation(handle, name_), count_, v_);
	}

	void SetUniform3fv(const char* name_, int count_, const float* v_)
	{
		glUniform3fv(glGetUniformLocation(handle, name_), count_, v_);
	}

	void SetUniform4fv(const char* name_, int count_, const float* v_)
	{
		glUniform4fv(glGetUniformLocation(handle, name_), count_, v_);
	}

	void SetUniformMatrix2fv(const char* name_, int count_, const float* v_)
	{
		glUniformMatrix2fv(glGetUniformLocation(handle, name_), count_, true, v_);
	}

	void SetUniformMatrix3fv(const char* name_, int count_, const float* v_)
	{
		glUniformMatrix3fv(glGetUniformLocation(handle, name_), count_, true, v_);
	}

	void SetUniformMatrix4fv(const char* name_, int count_, const float* v_)
	{
		glUniformMatrix4fv(glGetUniformLocation(handle, name_), count_, true, v_);
	}
private:
	void CheckCompileErrors(GLuint shader, std::string type)
	{
		GLint success;
		GLchar infoLog[1024];
		if (type != "PROGRAM")
		{
			glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
			if (!success)
			{
				glGetShaderInfoLog(shader, 1024, NULL, infoLog);
				std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
			}
		}
		else
		{
			glGetProgramiv(shader, GL_LINK_STATUS, &success);
			if (!success)
			{
				glGetProgramInfoLog(shader, 1024, NULL, infoLog);
				std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
			}
		}
	}
private:
	unsigned int handle;
};

#endif