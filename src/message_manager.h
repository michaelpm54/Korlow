#ifndef MESSAGE_MANAGER_H
#define MESSAGE_MANAGER_H

#include <chrono>
#include <string>
#include <vector>

struct Message
{
	std::string text;
	float x;
	float y;
	std::chrono::system_clock::time_point begin;
	std::chrono::duration<int, std::milli> duration;
};

class MessageManager
{
public:
	MessageManager();
	void addMessage(std::string msg, float x, float y, int durationMs);
	void update();
	const std::vector<Message> &getMessages() const;

private:
	std::vector<Message> mQueue;
};

#endif // MESSAGE_MANAGER_H
