#include "gui/opengl_widget.h"

#include <QOpenGLContext>

OpenGLWidget::OpenGLWidget(QWidget *parent)
	: QOpenGLWidget(parent)
{
}

OpenGLWidget::~OpenGLWidget()
{}

void OpenGLWidget::resizeGL(int w, int h)
{
	glViewport(0, 0, w, h);
}
