/*
Map: a 32x32 array of tile indices, which are 1 byte long.
a tile index refers to a location in the tile data starting at either
0x8000 or 0x9000 depending on whether LCDC.4 is set.
0 = 0x8000 and the tile index is unsigned
1 = 0x9000 and the tile index is signed
Each tile data set is 0x1000 bytes long.

For the map view I will create a texture showing an RGB colour from 0-255 based on the index,
for each index.

There are two maps of size 0x400. One is at 0x9800 and the other is at 0x9C00.
Which one is used is set based on LCDC.3.
*/

#include "render/map_scene.h"

#include <glm/gtc/matrix_transform.hpp>

#include "ppu_map_proxy.h"
#include "gl_util.h"
#include "textured_quad_gl.h"

#include "constants.h"

MapScene::MapScene(QWidget *parent)
	: OpenGLWidget(parent)
	, quad_program(GL_NONE)
	, map_view(std::make_unique<TexturedQuadGL>())
	, projection(glm::ortho(0.0f, static_cast<float>(kMapWidth), 0.0f, static_cast<float>(kMapHeight)))
{
	map_view->set_enabled(false);
}

void MapScene::set_have_rom(bool value)
{
	map_view->set_enabled(value);
	OpenGLWidget::update();
}

void MapScene::init()
{
	quad_program = glCreateProgram();
	loadShaders(quad_program, kQuadVertexShader, kQuadFragmentShader);

	map_view->create(kMapWidth, kMapHeight);
}

void MapScene::paintGL()
{
	map_view->draw(quad_program, projection);
}

void MapScene::update(PpuMapProxy *ppu_proxy)
{
	makeCurrent();
	map_view->set_pixels(ppu_proxy->get_map_pixels());
	OpenGLWidget::update();
	doneCurrent();
}

