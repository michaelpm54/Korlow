#ifndef KORLOW_MESSAGE_QUEUE_H
#define KORLOW_MESSAGE_QUEUE_H

#include <chrono>
#include <string>
#include <vector>

class MessageQueue {
public:
    void push(const std::string &msg, std::chrono::duration<int> duration);
    void update();
    void draw();

private:
    struct Message {
        std::string str;
        std::chrono::time_point<std::chrono::steady_clock> expire;
    };

    std::vector<Message> m_messages;
};

#endif    // KORLOW_MESSAGE_QUEUE_H
