#ifndef WINDOW_H
#define WINDOW_H

#include <string>
#include <vector>
#include <GLFW/glfw3.h>

class KeyReceiver;

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
	void addReceiver(KeyReceiver *r);

private:
	GLFWwindow *mHandle { nullptr };
	std::string mTitle { "" };
	int mWidth { 256 };
	int mHeight { 256 };
	bool mClosed { false };

	std::vector<KeyReceiver*> mReceivers;

	inline static void KeyCallback(GLFWwindow *handle, int key, int scancode, int action, int mods)
	{
        Window *window = static_cast<Window*>(glfwGetWindowUserPointer(handle));
        window->keyCallback(handle, key, scancode, action, mods);
    }
};

#endif // WINDOW_H
