#include "message_manager.h"

MessageManager::MessageManager()
{}

void MessageManager::addMessage(std::string msg, float x, float y, int durationMs)
{
	mQueue.push_back({
		msg,
		x,
		y,
		std::chrono::system_clock::now(),
		std::chrono::milliseconds(durationMs)
	});
}

const std::vector<Message>& MessageManager::getMessages() const
{
	return mQueue;
}

void MessageManager::update()
{
	auto now = std::chrono::system_clock::now();
	mQueue.erase(
		std::remove_if(
			mQueue.begin(),
			mQueue.end(),
			[=](auto &m)
			{
				return std::chrono::duration_cast<std::chrono::milliseconds>(now - m.begin) > m.duration;
			}
		),
		mQueue.end()
	);
}
