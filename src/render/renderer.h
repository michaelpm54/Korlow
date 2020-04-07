#ifndef RENDERER_H
#define RENDERER_H

class Renderer
{
public:
	virtual ~Renderer() = default;
	virtual void setup_opengl_resources() {};
	virtual void render() {};
	void set_enabled(bool value) { mEnabled = value; }

protected:
	bool mEnabled { true };
};

#endif // RENDERER_H
