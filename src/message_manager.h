#ifndef MESSAGE_MANAGER_H
#define MESSAGE_MANAGER_H

#include <chrono>
#include <string>
#include <vector>

#include "render/font/ft_font.h"

struct Message
{
	std::chrono::system_clock::time_point begin;
	std::chrono::duration<int, std::milli> duration;
	RenderedString str;
};

class MessageManager
{
public:
	MessageManager();
	void add_message(std::string msg, FTFont *font);
	void update();
	const std::vector<Message> &get_messages() const;

private:
	std::vector<Message> mQueue;
};

#endif // MESSAGE_MANAGER_H
