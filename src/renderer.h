#ifndef RENDERER_H
#define RENDERER_H

class Renderer
{
public:
	virtual ~Renderer() = default;
	virtual void initGL() = 0;
	virtual void render() = 0;
};

#endif // RENDERER_H
