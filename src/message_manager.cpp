#include "message_manager.h"

MessageManager::MessageManager()
{}

void MessageManager::add_message(std::string msg, FTFont *font)
{
	mQueue.push_back({
		std::chrono::system_clock::now(),
		std::chrono::milliseconds(1200),
		font->createString(msg, 64, 64 + mQueue.size() * 40)
	});
}

const std::vector<Message>& MessageManager::get_messages() const
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
