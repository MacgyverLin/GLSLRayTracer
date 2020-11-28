#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <glad\glad.h>
#include <GLFW\glfw3.h>

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 400;

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}
}

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
	}

	bool Create(const char* vertexPath, const char* fragmentPath)
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
			std::stringstream vShaderStream, fShaderStream;

			// read file's buffer contents into streams
			vShaderStream << vShaderFile.rdbuf();
			fShaderStream << fShaderFile.rdbuf();

			// close file handlers
			vShaderFile.close();
			fShaderFile.close();

			// convert stream into string
			vertexCode = vShaderStream.str();
			fragmentCode = fShaderStream.str();
		}
		catch (std::ifstream::failure&)
		{
			return false;
		}

		const char* vShaderCode = vertexCode.c_str();
		const char* fShaderCode = fragmentCode.c_str();

		// 2. compile shaders
		unsigned int vertex, fragment;
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

		// shader Program
		handle = glCreateProgram();
		glAttachShader(handle, vertex);
		glAttachShader(handle, fragment);

		glLinkProgram(handle);
		CheckCompileErrors(handle, "PROGRAM");
		// delete the shaders as they're linked into our program now and no longer necessery
		glDeleteShader(vertex);
		glDeleteShader(fragment);

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
		int location = glGetUniformLocation(handle, name_);

		glUniform1i(location, v0_);
	}

	void SetUniform2i(const char* name_, int v0_, int v1_)
	{
		int location = glGetUniformLocation(handle, name_);

		glUniform2i(location, v0_, v1_);
	}

	void SetUniform3i(const char* name_, int v0_, int v1_, int v2_)
	{
		int location = glGetUniformLocation(handle, name_);

		glUniform3i(location, v0_, v1_, v2_);
	}

	void SetUniform4i(const char* name_, int v0_, int v1_, int v2_, int v3_)
	{
		int location = glGetUniformLocation(handle, name_);

		glUniform4i(location, v0_, v1_, v2_, v3_);
	}

	void SetUniform1f(const char* name_, float v0_)
	{
		int location = glGetUniformLocation(handle, name_);

		glUniform1f(location, v0_);
	}

	void SetUniform2f(const char* name_, float v0_, float v1_)
	{
		int location = glGetUniformLocation(handle, name_);

		glUniform2f(location, v0_, v1_);
	}

	void SetUniform3f(const char* name_, float v0_, float v1_, float v2_)
	{
		int location = glGetUniformLocation(handle, name_);

		glUniform3f(location, v0_, v1_, v2_);
	}

	void SetUniform4f(const char* name_, float v0_, float v1_, float v2_, float v3_)
	{
		int location = glGetUniformLocation(handle, name_);

		glUniform4f(location, v0_, v1_, v2_, v3_);
	}

	void SetUniform1iv(const char* name_, int count_, const int* v_)
	{
		int location = glGetUniformLocation(handle, name_);

		glUniform1iv(location, count_, v_);
	}

	void SetUniform2iv(const char* name_, int count_, const int* v_)
	{
		int location = glGetUniformLocation(handle, name_);

		glUniform2iv(location, count_, v_);
	}

	void SetUniform3iv(const char* name_, int count_, const int* v_)
	{
		int location = glGetUniformLocation(handle, name_);

		glUniform3iv(location, count_, v_);
	}

	void SetUniform4iv(const char* name_, int count_, const int* v_)
	{
		int location = glGetUniformLocation(handle, name_);

		glUniform4iv(location, count_, v_);
	}

	void SetUniform1fv(const char* name_, int count_, const float* v_)
	{
		int location = glGetUniformLocation(handle, name_);

		glUniform1fv(location, count_, v_);
	}

	void SetUniform2fv(const char* name_, int count_, const float* v_)
	{
		int location = glGetUniformLocation(handle, name_);

		glUniform2fv(location, count_, v_);
	}

	void SetUniform3fv(const char* name_, int count_, const float* v_)
	{
		int location = glGetUniformLocation(handle, name_);

		glUniform3fv(location, count_, v_);
	}

	void SetUniform4fv(const char* name_, int count_, const float* v_)
	{
		int location = glGetUniformLocation(handle, name_);

		glUniform4fv(location, count_, v_);
	}

	void SetUniformMatrix2fv(const char* name_, int count_, const float* v_)
	{
		int location = glGetUniformLocation(handle, name_);

		glUniformMatrix2fv(location, count_, true, v_);
	}

	void SetUniformMatrix3fv(const char* name_, int count_, const float* v_)
	{
		int location = glGetUniformLocation(handle, name_);

		glUniformMatrix3fv(location, count_, true, v_);
	}

	void SetUniformMatrix4fv(const char* name_, int count_, const float* v_)
	{
		int location = glGetUniformLocation(handle, name_);

		glUniformMatrix4fv(location, count_, true, v_);
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
		//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		//glDrawArrays(GL_TRIANGLES, 0, 3);
		glDrawElements(type_, count_, GL_UNSIGNED_INT, 0);
	}
private:
	unsigned int VAO;
	unsigned int VBO;
	unsigned int EBO;
};

class Texture
{
public:
	Texture(unsigned int type_)
		: handle(0)
		, type(type_)
		, format(GL_RGBA)
		, pixelFormat(GL_UNSIGNED_BYTE)
	{
	}

	virtual ~Texture()
	{
	}

	void Bind(unsigned int texStage_)
	{
		if (handle)
		{
			glActiveTexture(GL_TEXTURE0 + texStage_);
			glBindTexture(type, handle);
		}
	}

	void Unbind()
	{
		glBindTexture(type, 0);
	}
protected:
	unsigned int handle;

	unsigned int type;
	unsigned int format;
	unsigned int pixelFormat;
};

class Texture2D : public Texture
{
public:
	Texture2D()
		: Texture(GL_TEXTURE_2D)
	{
	}

	~Texture2D()
	{
	}

	bool Create(unsigned int width, unsigned int height, unsigned int nrComponents, bool isHDR, void* data)
	{
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		if (isHDR)
			pixelFormat = GL_FLOAT;
		else
			pixelFormat = GL_UNSIGNED_BYTE;

		glGenTextures(1, &handle);
		glBindTexture(GL_TEXTURE_2D, handle);
		glTexImage2D(GL_TEXTURE_2D, 0, (GLint)format, width, height, 0, (GLint)format, (GLint)pixelFormat, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		return true;
	}

	bool Create(char const* path)
	{
		bool isHDR = stbi_is_hdr(path);

		int width, height, nrComponents;

		void* data = nullptr;
		if (isHDR)
			data = stbi_loadf(path, &width, &height, &nrComponents, 0);
		else
			data = stbi_load(path, &width, &height, &nrComponents, 0);

		if (data)
		{
			bool result = Create(width, height, nrComponents, isHDR, data);

			stbi_image_free(data);

			return result;
		}
		else
		{
			return false;
		}
	}

	void Destroy()
	{
		if (handle)
		{
			glDeleteTextures(1, &handle);
			handle = 0;
		}
	}
private:
private:
};


ShaderProgram shaderProgram;
Texture2D diffuseMap;
Texture2D specularMap;
Texture2D envMap;
VertexArrayObject vertexArrayObject;

bool createScene()
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

	shaderProgram.Bind();

	if (!diffuseMap.Create("../assets/diffuseMap.png"))
	{
		return false;
	}

	if (!specularMap.Create("../assets/specularMap.png"))
	{
		return false;
	}

	if (!envMap.Create("../assets/photo_studio_01_1k.hdr"))
	{
		return false;
	}

	return true;
}

void renderScene()
{
	glClearColor(0.0f, 0.5f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	shaderProgram.Bind();
	shaderProgram.SetUniform1i("diffuseMap", 0);
	shaderProgram.SetUniform1i("specularMap", 1);
	shaderProgram.SetUniform1i("envMap", 2);
	shaderProgram.SetUniform2f("screenSize", SCR_WIDTH, SCR_HEIGHT);

	vertexArrayObject.Bind();

	diffuseMap.Bind(0);
	specularMap.Bind(1);
	envMap.Bind(2);

	vertexArrayObject.Draw(GL_TRIANGLES, 6);
}

void destroyScene()
{
	diffuseMap.Destroy();

	specularMap.Destroy();

	vertexArrayObject.Destroy();

	shaderProgram.Destroy();
}

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	if (!createScene())
	{
		std::cout << "Failed to init Scene" << std::endl;
		return -1;
	}

	while (!glfwWindowShouldClose(window))
	{
		processInput(window);

		renderScene();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	destroyScene();
	glfwTerminate();

	return 0;
}