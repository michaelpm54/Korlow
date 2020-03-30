#ifndef MESSAGE_RENDERER_H
#define MESSAGE_RENDERER_H

#include <chrono>
#include <map>

#include "render/renderer.h"
#include "render/font/ft_font.h"

class MessageManager;

class MessageRenderer : public Renderer
{
public:
	MessageRenderer(MessageManager *msgMan, FTFont *font);
	~MessageRenderer();

	void initGL() override;
	void render() override;
	void setFont(FTFont *font);

private:
	MessageManager *mMessageManager { nullptr };
	FTFont *mFont { nullptr };
	std::map<std::chrono::system_clock::time_point, RenderedString> mRenderedMessages;
};

#endif // MESSAGE_RENDERER_H
