#ifndef OPENGL_WIDGET_H
#define OPENGL_WIDGET_H

#include "render/opengl.h"

#include <QOpenGLWidget>

class OpenGLWidget : public QOpenGLWidget
{
public:
	OpenGLWidget(QWidget *parent = nullptr);
	virtual ~OpenGLWidget();

protected:
	void resizeGL(int w, int h) override;
	virtual void paintGL() = 0;
	virtual void initializeGL() = 0;
};

#endif // OPENGL_WIDGET_H
