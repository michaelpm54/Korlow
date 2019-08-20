#include "window.h"

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
}

void Window::keyCallback(GLFWwindow *handle, int key, int scancode, int action, int mods)
{
	if ((key == GLFW_KEY_Q || key == GLFW_KEY_ESCAPE) && action == GLFW_PRESS)
	{
		mClosed = true;
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
