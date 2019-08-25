#ifndef KEY_RECEIVER_H
#define KEY_RECEIVER_H

class KeyReceiver
{
public:
	virtual void sendKey(int key, int scancode, int action, int mods) = 0;

protected:
	virtual ~KeyReceiver() = default;
};

#endif // KEY_RECEIVER_H
