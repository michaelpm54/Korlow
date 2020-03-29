#ifndef MESSAGE_RENDERER_H
#define MESSAGE_RENDERER_H

#include "renderer.h"

class Font;
class MessageManager;

class MessageRenderer : public Renderer
{
public:
	MessageRenderer(MessageManager *msgMan);

	void initGL() override;
	void render() override;
	void setFont(Font *font);

private:
	MessageManager *mMessageManager { nullptr };
	Font *mFont { nullptr };
};

#endif // MESSAGE_RENDERER_H
