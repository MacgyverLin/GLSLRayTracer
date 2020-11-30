#ifndef _Texture_h_
#define _Texture_h_

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <glad\glad.h>
#include <GLFW\glfw3.h>
#include <vector>

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
	unsigned int internalformat;
	unsigned int pixelFormat;
};

class Texture2D : public Texture
{
public:
	Texture2D()
		: Texture(GL_TEXTURE_2D)
	{
	}

	virtual ~Texture2D()
	{
	}

	bool Create(unsigned int width, unsigned int height, unsigned int nrComponents, bool isHDR, void* data)
	{
		int error = 0;
		if (nrComponents == 1)
		{
			format = GL_RED;
			if (isHDR)
			{
				internalformat = GL_R16;
			}
			else
			{
				internalformat = GL_R16F;
			}
		}
		else if (nrComponents == 2)
		{
			format = GL_RG;
			if (isHDR)
			{
				internalformat = GL_RG16;
			}
			else
			{
				internalformat = GL_RG16F;
			}
		}
		else if (nrComponents == 3)
		{
			format = GL_RGB;
			if (isHDR)
			{
				internalformat = GL_RGB32F;
			}
			else
			{
				internalformat = GL_RGB;
			}
		}
		else if (nrComponents == 4)
		{
			format = GL_RGBA;
			if (isHDR)
			{
				internalformat = GL_RGBA32F;
			}
			else
			{
				internalformat = GL_RGBA;
			}
		}

		if (isHDR)
			pixelFormat = GL_FLOAT;
		else
			pixelFormat = GL_UNSIGNED_BYTE;

		error = glGetError();
		glGenTextures(1, &handle);
		
		error = glGetError();
		glBindTexture(GL_TEXTURE_2D, handle);
		
		error = glGetError();
		glTexImage2D(GL_TEXTURE_2D, 0, (GLint)internalformat, width, height, 0, (GLint)format, (GLint)pixelFormat, data);

		error = glGetError();
		glGenerateMipmap(GL_TEXTURE_2D);

		error = glGetError();
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

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


class RandomTexture2D : public Texture2D
{
public:
	RandomTexture2D()
		: Texture2D()
	{
	}

	virtual ~RandomTexture2D()
	{
	}

	bool Create(int seed = 0)
	{
#define RAND_TEX_SIZE 1048
		std::vector<float> data;
		data.resize(RAND_TEX_SIZE * RAND_TEX_SIZE);

		for (int i = 0; i < RAND_TEX_SIZE * RAND_TEX_SIZE; i++)
		{
			data[i] = float(rand()) / RAND_MAX;
		}
		
		return Texture2D::Create(RAND_TEX_SIZE, RAND_TEX_SIZE, 1, true, &data[0]);
	}
private:
private:
};

#endif