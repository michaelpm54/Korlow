#ifndef OPENGL_WIDGET_H
#define OPENGL_WIDGET_H

#include "opengl.h"

#include <QOpenGLWidget>
#include <QOpenGLFunctions>

#include <glm/glm.hpp>

class OpenGLWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
public:
	OpenGLWidget(QWidget *parent = nullptr);
	~OpenGLWidget();

	void updateMap(const uint8_t *data);
	void update(const uint8_t *pixels);

protected:
	void initializeGL() override;
	void resizeGL(int w, int h) override;
	void paintGL() override;

private:
	void createGLObjects();
	void destroyGLObjects();

	GLuint mProgram { 0 };
	GLuint mFrameTexture { 0 };
	GLuint mVao { 0 };
	GLuint mVbo { 0 };

	int mWidth { 0 };
	int mHeight { 0 };
	int mNumPixels { 0 };

	GLuint mMapTex;
	GLuint mMapBuf;
	GLuint mMapVao;

	glm::mat4 mProjMatrix { 1.0f };
};

#endif // OPENGL_WIDGET_H
