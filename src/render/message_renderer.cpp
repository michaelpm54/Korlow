#include "render/message_renderer.h"

#include "message_manager.h"
#include "render/font/ft_util.h"

MessageRenderer::MessageRenderer(MessageManager *msgMan, FTFont *font)
	: message_manager(msgMan)
	, font(font)
{
	FTUtil::Init();
}

MessageRenderer::~MessageRenderer()
{
	FTUtil::Done();
}

void MessageRenderer::setup_opengl_resources()
{}

void MessageRenderer::render()
{
	if (!font)
	{
		return;
	}

	for (const auto& msg : message_manager->getMessages())
	{
		if (mRenderedMessages.count(msg.begin))
			font->drawString(mRenderedMessages[msg.begin]);
		else
			mRenderedMessages.insert({msg.begin, font->createString(msg.text, msg.x, msg.y)});
	}
}

void MessageRenderer::set_font(FTFont* font)
{
	font = font;
}
