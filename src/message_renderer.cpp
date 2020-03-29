#include "message_renderer.h"

#include "font.h"
#include "message_manager.h"

MessageRenderer::MessageRenderer(MessageManager *msgMan)
	: mMessageManager(msgMan)
{}

void MessageRenderer::initGL()
{}

void MessageRenderer::render()
{
	if (!mFont)
	{
		return;
	}

	for (const auto& msg : mMessageManager->getMessages())
	{
		mFont->renderText(msg.text, msg.x, msg.y);
	}
}

void MessageRenderer::setFont(Font* font)
{
	mFont = font;
}
