#ifndef _FrameWork_h_
#define _FrameWork_h_

#include <glad\glad.h>
#include <GLFW\glfw3.h>
#include <iostream>
#include <assert.h>
#include <string>
#include <map>

class FrameWork;
static FrameWork* instance = nullptr;

class FrameWork
{
public:
	FrameWork()
		: theta(0.0)
		, phi(0.0)
	{
		assert(!instance);

		instance = this;
	}

	virtual ~FrameWork()
	{
		instance = nullptr;
	}

	bool Create(int width_, int height_)
	{
		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
		width = width_;
		height = height_;
		window = glfwCreateWindow(width, height, "GLSL Ray Tracing", NULL, NULL);
		if (window == NULL)
		{
			std::cout << "Failed to create GLFW window" << std::endl;
			glfwTerminate();
			return false;
		}
		glfwMakeContextCurrent(window);
		glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			std::cout << "Failed to initialize GLAD" << std::endl;
			return false;
		}

		if (!OnCreate())
		{
			std::cout << "Failed to init Scene" << std::endl;
			return false;
		}

		return true;
	}

	bool Start()
	{
		while (!glfwWindowShouldClose(window))
		{
			processInput(window);

			if (!OnUpdate())
				return false;

			glfwSwapBuffers(window);
			glfwPollEvents();
		}

		return true;
	}

	void Destroy()
	{
		OnDestroy();

		glfwTerminate();
	}
protected:
	bool IsKeyPressed(char key)
	{
		return glfwGetKey(window, GLFW_KEY_0 + key - '0') == GLFW_PRESS;
	}

	bool IsKeyRelease(char key)
	{
		return glfwGetKey(window, GLFW_KEY_0 + key - '0') == GLFW_RELEASE;
	}

	bool IsKeyRepeat(char key)
	{
		return glfwGetKey(window, GLFW_KEY_0 + key - '0') == GLFW_REPEAT;
	}

	void GetTheta(float& theta_, float& phi_)
	{
		theta_ = theta;
		phi_ = phi;
	}

	virtual bool OnCreate() = 0;
	virtual bool OnUpdate() = 0;
	virtual void OnDestroy() = 0;
private:
	static void framebuffer_size_callback(GLFWwindow* window, int width, int height)
	{
		glViewport(0, 0, width, height);
	}

	static void processInput(GLFWwindow* window)
	{
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		{
			glfwSetWindowShouldClose(window, true);
		}

		static float lastXPos = 0;
		static float lastYPos = 0;
		double xpos = 0;
		double ypos = 0;
		glfwGetCursorPos(window, &xpos, &ypos);

		float deltaX = xpos - lastXPos;
		float deltaY = ypos - lastYPos;

		instance->moved = (fabs(deltaX) < 0.01f) && (fabs(deltaX) < 0.01f);
		lastXPos = xpos;
		lastYPos = ypos;
		instance->theta += deltaX / instance->width * 180.0f * 2.0f;
		instance->phi -= deltaY / instance->height * 180.0f;
		instance->theta = fmod(instance->theta, 180.0f * 2.0f);

		if (instance->phi > 70)
		{
			instance->phi = 70;
		}
		else if (instance->phi < -70)
		{
			instance->phi = -70;
		}
	}
protected:
	bool moved;

private:
	GLFWwindow* window;
	int width;
	int height;

	float theta;
	float phi;
};

#endif