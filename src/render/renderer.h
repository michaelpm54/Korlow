#ifndef RENDERER_H
#define RENDERER_H

class Renderer
{
public:
	virtual ~Renderer() = default;
	virtual void initGL() {};
	virtual void render() {};
	void setEnabled(bool value) { mEnabled = value; }

protected:
	bool mEnabled { true };
};

#endif // RENDERER_H
