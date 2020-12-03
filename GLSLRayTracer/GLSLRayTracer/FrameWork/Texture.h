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
		, internalformat(GL_RGBA)
		, pixelFormat(GL_UNSIGNED_BYTE)

		, warpS(GL_REPEAT)
		, warpT(GL_REPEAT)
		, warpR(GL_REPEAT)

		, minFilter(GL_NEAREST)
		, magFilter(GL_NEAREST)
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

			glTexParameteri(type, GL_TEXTURE_WRAP_S, warpS);
			glTexParameteri(type, GL_TEXTURE_WRAP_T, warpT);
			glTexParameteri(type, GL_TEXTURE_WRAP_R, warpR);
			glTexParameteri(type, GL_TEXTURE_MIN_FILTER, minFilter);
			glTexParameteri(type, GL_TEXTURE_MAG_FILTER, magFilter);
		}
	}

	void Unbind()
	{
		glBindTexture(type, 0);
	}

	void SetWarpS(unsigned int warpS_)
	{
		warpS = warpS_;
	}

	void SetWarpT(unsigned int warpT_)
	{
		warpT = warpT_;
	}

	void SetWarpR(unsigned int warpR_)
	{
		warpR = warpR_;
	}

	void SetMinFilter(unsigned int minFilter_)
	{
		minFilter = minFilter_;
	}

	void SetMagFilter(unsigned int magFilter_)
	{
		magFilter = magFilter_;
	}

	unsigned int GetWarpS() const
	{
		return warpS;
	}

	unsigned int GetWarpT() const
	{
		return warpT;
	}

	unsigned int GetWarpR() const
	{
		return warpR;
	}

	unsigned int GetMinFilter()
	{
		return minFilter;
	}

	unsigned int GetMagFilter()
	{
		return magFilter;
	}
public:
protected:
	void SetFormat(unsigned int nrComponents, bool isHDR)
	{
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
	}
private:


public:
protected:
	unsigned int handle;

	unsigned int type;
	unsigned int format;
	unsigned int internalformat;
	unsigned int pixelFormat;

	unsigned int warpS;
	unsigned int warpR;
	unsigned int warpT;
	unsigned int minFilter;
	unsigned int magFilter;
private:
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
		Destroy();
	}

	bool Create(unsigned int width, unsigned int height, unsigned int nrComponents, bool isHDR, void* data)
	{
		SetFormat(nrComponents, isHDR);

		glGenTextures(1, &handle);
		glBindTexture(GL_TEXTURE_2D, handle);
		glTexImage2D(GL_TEXTURE_2D, 0, (GLint)internalformat, width, height, 0, (GLint)format, (GLint)pixelFormat, data);
		glGenerateMipmap(GL_TEXTURE_2D);

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
protected:
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
#define RAND_TEX_SIZE 1024
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

class FrameBufferTexture2D : public Texture
{
public:
	FrameBufferTexture2D()
		: Texture(GL_TEXTURE_2D)
		, fbo(0)
	{
	}

	virtual ~FrameBufferTexture2D()
	{
		Destroy();
	}

	bool Create(unsigned int width, unsigned int height, unsigned int nrComponents, bool isHDR)
	{
		SetFormat(nrComponents, isHDR);
		
		glGenFramebuffers(1, &fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		if (!glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)
		{
			return false;
		}

		// generate texture
		glGenTextures(1, &handle);
		glBindTexture(GL_TEXTURE_2D, handle);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, handle, 0);

		glGenRenderbuffers(1, &rbo);
		glBindRenderbuffer(GL_RENDERBUFFER, rbo);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height); // use a single renderbuffer object for both a depth AND stencil buffer.
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo); // now actually attach it
		// now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			return false;

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		return true;
	}

	void Destroy()
	{
		if (fbo)
		{
			glDeleteFramebuffers(1, &fbo);
			fbo = 0;
		}
	}

	void BindFrameBuffer()
	{
		if (fbo)
		{
			glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		}
	}

	void UnBindFrameBuffer()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
private:
private:
	unsigned int fbo;
	unsigned int rbo;
};

class TextureCube : public Texture
{
public:
	TextureCube()
		: Texture(GL_TEXTURE_CUBE_MAP)
	{
	}

	virtual ~TextureCube()
	{
		Destroy();
	}

	bool Create(unsigned int size, unsigned int nrComponents, bool isHDR, void* data)
	{
		SetFormat(nrComponents, isHDR);

		glGenTextures(1, &handle);
		glBindTexture(GL_TEXTURE_CUBE_MAP, handle);

		int dataSize;
		if (isHDR)
			dataSize = size * size * nrComponents * sizeof(float);
		else
			dataSize = size * size * nrComponents * sizeof(unsigned char);

		unsigned char* dataPtr = (unsigned char*)data;
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, (GLint)internalformat, size, size, 0, (GLint)format, (GLint)pixelFormat, dataPtr); dataPtr += dataSize;
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, (GLint)internalformat, size, size, 0, (GLint)format, (GLint)pixelFormat, dataPtr); dataPtr += dataSize;
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, (GLint)internalformat, size, size, 0, (GLint)format, (GLint)pixelFormat, dataPtr); dataPtr += dataSize;
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, (GLint)internalformat, size, size, 0, (GLint)format, (GLint)pixelFormat, dataPtr); dataPtr += dataSize;
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, (GLint)internalformat, size, size, 0, (GLint)format, (GLint)pixelFormat, dataPtr); dataPtr += dataSize;
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, (GLint)internalformat, size, size, 0, (GLint)format, (GLint)pixelFormat, dataPtr); dataPtr += dataSize;
		glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

		warpS = GL_CLAMP_TO_EDGE;
		warpT = GL_CLAMP_TO_EDGE;
		warpR = GL_CLAMP_TO_EDGE;

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
			bool result = Create(width, nrComponents, isHDR, data);

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

#endif