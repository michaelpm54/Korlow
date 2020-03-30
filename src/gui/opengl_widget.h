#ifndef OPENGL_WIDGET_H
#define OPENGL_WIDGET_H

#include "render/opengl.h"

#include <QOpenGLWidget>
#include <QOpenGLFunctions>

class Renderer;

class OpenGLWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
public:
	OpenGLWidget(QWidget *parent = nullptr);

	void addRenderer(Renderer *renderer);

protected:
	void initializeGL() override;
	void resizeGL(int w, int h) override;
	void paintGL() override;

private:
	std::vector<Renderer*> mRenderers;

	int mWidth { 0 };
	int mHeight { 0 };
};

#endif // OPENGL_WIDGET_H
