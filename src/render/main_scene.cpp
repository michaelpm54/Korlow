#include "render/main_scene.h"

#include <glm/gtc/matrix_transform.hpp>

#include "gameboy.h"
#include "gl_util.h"
#include "textured_quad_gl.h"
#include "constants.h"

MainScene::MainScene(QWidget *parent)
	: OpenGLWidget(parent)
	, quad_program(GL_NONE)
	, lcd(std::make_unique<TexturedQuadGL>())
	, projection(glm::ortho(0.0f, static_cast<float>(kLcdWidth), 0.0f, static_cast<float>(kLcdHeight)))
{
	lcd->set_enabled(false);
}

void MainScene::set_have_rom(bool value)
{
	lcd->set_enabled(value);
	OpenGLWidget::update();
}

void MainScene::init()
{
	quad_program = glCreateProgram();
	loadShaders(quad_program, kQuadVertexShader, kQuadFragmentShader);

	lcd->create(kLcdWidth, kLcdHeight);
}

void MainScene::paintGL()
{
	lcd->draw(quad_program, projection);
}

void MainScene::update(Gameboy* gameboy)
{
	makeCurrent();
	lcd->set_pixels(gameboy->get_lcd_pixels());
	OpenGLWidget::update();
	doneCurrent();
}
