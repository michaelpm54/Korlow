#version 460 core
layout(location = 0) in vec2 aPos;
layout(location = 1) in vec2 aTexCoord;
out vec2 texCoord;
uniform mat4 mvp;
void main()
{
	gl_Position = mvp * vec4(aPos, 0, 1);
	texCoord = aTexCoord;
}
