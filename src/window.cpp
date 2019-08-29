#include "window.h"

#include <cstdio>
#include <stdexcept>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "key_receiver.h"

void KeyCallback(GLFWwindow *handle, int key, int scancode, int action, int mods)
{
    Window *window = static_cast<Window*>(glfwGetWindowUserPointer(handle));
    window->keyCallback(handle, key, scancode, action, mods);
}

void resizeCallback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}

Window::~Window()
{
	glfwDestroyWindow(mHandle);
	glfwTerminate();
}

void Window::setTitle(std::string title)
{
	mTitle = std::move(title);
	if (mHandle)
	{
		glfwSetWindowTitle(mHandle, mTitle.c_str());
	}
}

void Window::setSize(int width, int height)
{
	mWidth = width;
	mHeight = height;
	if (mHandle)
	{
		glfwSetWindowSize(mHandle, mWidth, mHeight);
	}
}

void Window::create()
{
	if (mHandle)
	{
		glfwDestroyWindow(mHandle);
	}
	else
	{
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_REFRESH_RATE, 60);
	}
	mHandle = glfwCreateWindow(mWidth, mHeight, mTitle.c_str(), nullptr, nullptr);
	glfwMakeContextCurrent(mHandle);
	glfwSetWindowUserPointer(mHandle, this);
	glfwSetKeyCallback(mHandle, KeyCallback);
	glfwSetFramebufferSizeCallback(mHandle, resizeCallback);
	if (!mDoneGlewInit)
	{
		GLenum err = glewInit();
		if (GLEW_OK != err)
		{
  			throw std::runtime_error("Error: " + std::string(reinterpret_cast<const char*>(glewGetErrorString(err))));
		}
		mDoneGlewInit = true;
	}
}

void Window::keyCallback(GLFWwindow *handle, int key, int scancode, int action, int mods)
{
	if ((key == GLFW_KEY_Q || key == GLFW_KEY_ESCAPE) && action == GLFW_PRESS)
	{
		mClosed = true;
	}

	for (auto &r : mReceivers)
	{
		r->sendKey(key, scancode, action, mods);
	}
}

void Window::events()
{
	if (glfwWindowShouldClose(mHandle))
	{
		mClosed = true;
	}
	glfwPollEvents();
}

void Window::refresh()
{
	glfwSwapBuffers(mHandle);
}

bool Window::closed() const
{
	return mClosed;
}

void Window::addReceiver(KeyReceiver *r)
{
	mReceivers.push_back(r);
}
