#include "gui/opengl_widget.h"

#include <QOpenGLContext>
#include <stdexcept>

#include "constants.h"

OpenGLWidget::OpenGLWidget(QWidget *parent)
	: QOpenGLWidget(parent)
{
}

OpenGLWidget::~OpenGLWidget()
{}

void OpenGLWidget::initializeGL()
{
	if (!gladLoadGL())
		throw std::runtime_error("Failed to load GL functions");

	glClearColor(kClearColor[0], kClearColor[1], kClearColor[2], 1.0f);

	init();
}

void OpenGLWidget::resizeGL(int w, int h)
{
	glViewport(0, 0, w, h);
}
