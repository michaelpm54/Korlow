#version 460 core
in vec2 texCoord;
out vec4 colour;
uniform usampler2D tex;
void main()
{
	float c = texture(tex, texCoord).r / 255.0f;

	colour = vec4(0.4, c, 0.4, 1);
}