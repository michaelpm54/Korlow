#version 460 core
in vec2 texCoord;
out vec4 colour;
uniform sampler2D tex;
void main()
{
	colour = vec4(1, 1, 1, texture(tex, texCoord).r) * vec4(0.1, 0.7, 0.1, 1);
}
