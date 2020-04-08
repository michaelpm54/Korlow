#include "render/main_scene.h"

#include <glm/gtc/matrix_transform.hpp>

#include "gameboy.h"
#include "gl_util.h"
#include "textured_quad_gl.h"
#include "constants.h"
#include "message_manager.h"
#include "render/font/ft_font.h"

#include "render/font/ft_util.h"

MainScene::MainScene(QWidget *parent)
	: OpenGLWidget(parent)
	, quad_program(GL_NONE)
	, lcd(std::make_unique<TexturedQuadGL>())
	, projection(glm::ortho(0.0f, static_cast<float>(kLcdWidth), 0.0f, static_cast<float>(kLcdHeight)))
	, message_manager(std::make_unique<MessageManager>())
	, font(std::make_unique<FTFont>())
{
	lcd->set_enabled(false);
}

MainScene::~MainScene()
{
	makeCurrent();
	glDeleteProgram(quad_program);
	glDeleteProgram(text_program);
	FTUtil::Done();
	makeCurrent();
}

void MainScene::set_have_rom(bool value)
{
	lcd->set_enabled(value);
	OpenGLWidget::update();
}

void MainScene::add_message(const std::string& text)
{
	makeCurrent();
	message_manager->add_message(text, font.get());
	doneCurrent();
}

void MainScene::init()
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glPixelStorei(GL_PACK_ALIGNMENT, 1);

	quad_program = glCreateProgram();
	text_program = glCreateProgram();
	loadShaders(quad_program, kQuadVertexShader, kQuadFragmentShader);
	loadShaders(text_program, kTextVertexShader, kTextFragmentShader);

	FTUtil::Init();

	/*
		"ndsbios_memesbruh03.ttf",
		"SwissSiena.ttf",
	*/

	font->load("E:/Projects/Emulators/GB/Korlow2/assets/fonts/ndsbios_memesbruh03.ttf", 18);

	lcd->create(kLcdWidth, kLcdHeight);
}

void MainScene::paintGL()
{
	lcd->draw(quad_program, projection);

	for (auto &m : message_manager->get_messages())
	{
		font->drawString(text_program, m.str);
	}
}

void MainScene::set_lcd_pixels(const uint8_t *pixels)
{
	makeCurrent();
	lcd->set_pixels(pixels);
	doneCurrent();
	OpenGLWidget::update();
}

void MainScene::update()
{
	message_manager->update();
	OpenGLWidget::update();
}
