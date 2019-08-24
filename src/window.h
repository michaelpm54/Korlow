#ifndef WINDOW_H
#define WINDOW_H

#include <string>
#include <GLFW/glfw3.h>

class Window
{
public:
	Window() = default;
	~Window();
	void setTitle(std::string title);
	void setSize(int width, int height);
	void create();
	void events();
	void refresh();
	bool closed() const;
	void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);

private:
	GLFWwindow *mHandle { nullptr };
	std::string mTitle { "" };
	int mWidth { 256 };
	int mHeight { 256 };
	bool mClosed { false };

	inline static void KeyCallback(GLFWwindow *handle, int key, int scancode, int action, int mods)
	{
        Window *window = static_cast<Window*>(glfwGetWindowUserPointer(handle));
        window->keyCallback(handle, key, scancode, action, mods);
    }
};

#endif // WINDOW_H
