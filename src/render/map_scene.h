#ifndef MAP_SCENE_H
#define MAP_SCENE_H

#include "gui/opengl_widget.h"

#include <glm/glm.hpp>

struct PpuMapProxy;
class TexturedQuadGL;

class MapScene : public OpenGLWidget
{
public:
	MapScene(QWidget *parent = nullptr);
	void update(PpuMapProxy *ppu_proxy);
	void set_have_rom(bool);

protected:
	void paintGL() override;
	void init() override;

private:
	GLuint quad_program;
	std::unique_ptr<TexturedQuadGL> map_view;
	glm::mat4 projection;
};

#endif // MAP_SCENE_H
