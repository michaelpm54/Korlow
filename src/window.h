#ifndef WINDOW_H
#define WINDOW_H

#include <string>
#include <vector>

class KeyReceiver;
struct GLFWwindow;

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
	void addReceiver(KeyReceiver *r);

private:
	void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);

private:
	GLFWwindow *mHandle { nullptr };
	std::string mTitle { "" };
	int mWidth { 256 };
	int mHeight { 256 };
	bool mClosed { false };
	bool mDoneGlewInit { false };

	std::vector<KeyReceiver*> mReceivers;

	friend void KeyCallback(GLFWwindow *handle, int key, int scancode, int action, int mods);
};

#endif // WINDOW_H
