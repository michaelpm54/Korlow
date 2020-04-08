#ifndef MAIN_SCENE_H
#define MAIN_SCENE_H

#include "gui/opengl_widget.h"

#include <glm/glm.hpp>

struct Gameboy;
class FTFont;
class MessageManager;
class TexturedQuadGL;

class MainScene : public OpenGLWidget
{
public:
	MainScene(QWidget *parent = nullptr);
	~MainScene();
	void set_lcd_pixels(const uint8_t *pixels);
	void set_have_rom(bool);
	void add_message(const std::string& text);
	void update();

protected:
	void paintGL() override;
	void init() override;

private:
	GLuint text_program;
	GLuint quad_program;
	std::unique_ptr<TexturedQuadGL> lcd;
	glm::mat4 projection;

	std::unique_ptr<MessageManager> message_manager;
	std::unique_ptr<FTFont> font;
};

#endif // MAIN_SCENE_H

