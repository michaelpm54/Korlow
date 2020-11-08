#version 460 core
layout(location=0) in vec4 aPos;
layout(location=1) in vec2 aTexCoord;
uniform mat4 projection;
uniform mat4 model;
out vec2 texCoord;
void main()
{
	gl_Position = projection * model * aPos;
	texCoord = aTexCoord;
}