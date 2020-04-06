#ifndef MAP_RENDERER_H
#define MAP_RENDERER_H

#include <glm/glm.hpp>

#include "opengl.h"
#include "renderer.h"

class MapRenderer : public Renderer
{
public:
	MapRenderer();
	~MapRenderer();

	void initGL() override;
	void render() override;

	void update(const uint8_t *data);

private:
	void createGLObjects();
	void destroyGLObjects();

private:
	GLuint mProgram { 0 };
	GLuint mVao { 0 };
	GLuint mVbo { 0 };

	GLuint mMapTex;
	GLuint mMapBuf;
	GLuint mMapVao;

	glm::mat4 proj_matrix { 1.0f };
	glm::mat4 model_matrix { 1.0f };
};

#endif // MAP_RENDERER_H
