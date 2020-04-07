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

	void setup_opengl_resources() override;
	void render() override;
	void set_font(FTFont *font);

private:
	MessageManager *message_manager { nullptr };
	FTFont *font { nullptr };
	std::map<std::chrono::system_clock::time_point, RenderedString> mRenderedMessages;
};

#endif // MESSAGE_RENDERER_H
