#include "gui/opengl_widget.h"

#include <iostream>
#include <QApplication>

#include "render/renderer.h"

constexpr int kScreenScale = 3;
constexpr int kScreenSizeX = 160 * kScreenScale + (32 * 2 * kScreenScale);
constexpr int kScreenSizeY = 144 * kScreenScale;

OpenGLWidget::OpenGLWidget(QWidget *parent)
	: QOpenGLWidget(parent)
{
	QSurfaceFormat format;
	format.setDepthBufferSize(24);
	format.setStencilBufferSize(8);
	format.setVersion(4, 6);
	format.setProfile(QSurfaceFormat::CoreProfile);
	setFormat(format);
}

void OpenGLWidget::initializeGL()
{
	initializeOpenGLFunctions();

	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if (GLEW_OK != err)
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));

	glClearColor(0.0f, 0.0f, 0.5f, 1.0f);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	
	resizeGL(kScreenSizeX, kScreenSizeY);
}

void OpenGLWidget::resizeGL(int w, int h)
{
	mWidth = w;
	mHeight = h;
	glViewport(0, 0, w, h);
}

void OpenGLWidget::paintGL()
{
	glClear(GL_COLOR_BUFFER_BIT);
	for (Renderer* renderer : mRenderers)
	{
		renderer->render();
	}
}

void OpenGLWidget::addRenderer(Renderer* renderer)
{
	mRenderers.push_back(renderer);
}
