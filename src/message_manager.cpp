#include "message_manager.h"

MessageManager::MessageManager()
{}

void MessageManager::addMessage(std::string msg, float x, float y, int durationMs)
{
	if (mQueue.size())
		y = mQueue.back().y + 40;

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

	for (auto& msg : mQueue)
	{
		if (std::chrono::duration_cast<std::chrono::milliseconds>(now - msg.begin) > msg.duration)
		{
			printf("Removing message: %s\n", msg.text.c_str());
		}
	}

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
