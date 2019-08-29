#version 460 core
in vec2 texCoord;
out vec4 colour;
uniform usampler2D tex;
void main()
{
	float c = texture(tex, texCoord).r;
	c /= 255.0f;
	c = 0.8f - c;
	colour = vec4(c,c,c, 1);
}
