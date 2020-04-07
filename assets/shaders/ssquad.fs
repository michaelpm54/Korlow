#version 460 core
in vec2 texCoord;
out vec4 colour;
uniform usampler2D tex;
void main()
{
	float c = texture(tex, texCoord).r / 255.0f;

	float r = c * 0.8f;
	float g = c * 0.6f;
    float b = c * 0.4f;

	vec3 rgb = vec3(r, g, b);

	//c /= 255.0f;
	//c = 0.8f - c;

	colour = vec4(rgb, 1);
}