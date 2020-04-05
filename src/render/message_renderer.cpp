#include "render/message_renderer.h"

#include "message_manager.h"
#include "render/font/ft_util.h"

MessageRenderer::MessageRenderer(MessageManager *msgMan, FTFont *font)
	: mMessageManager(msgMan)
	, mFont(font)
{
	FTUtil::Init();
}

MessageRenderer::~MessageRenderer()
{
	FTUtil::Done();
}

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
		if (mRenderedMessages.count(msg.begin))
			mFont->drawString(mRenderedMessages[msg.begin]);
		else
			mRenderedMessages.insert({msg.begin, mFont->createString(msg.text, msg.x, msg.y)});
	}
}

void MessageRenderer::setFont(FTFont* font)
{
	mFont = font;
}
