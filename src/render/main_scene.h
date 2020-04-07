#ifndef MAIN_SCENE_H
#define MAIN_SCENE_H

#include "gui/opengl_widget.h"

#include <glm/glm.hpp>

struct Gameboy;
class TexturedQuadGL;

class MainScene : public OpenGLWidget
{
public:
	MainScene(QWidget *parent = nullptr);
	void update(Gameboy *gameboy);
	void set_have_rom(bool);

protected:
	void paintGL() override;
	void initializeGL() override;

private:
	GLuint quad_program;
	std::unique_ptr<TexturedQuadGL> lcd;
	glm::mat4 projection;
};

#endif // MAIN_SCENE_H

