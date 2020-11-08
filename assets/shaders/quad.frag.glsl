#version 460 core
in vec2 texCoord;
out vec4 colour;
uniform usampler2D tex;
void main()
{
    float c = texture(tex, texCoord).r / 255.0f;

    colour = vec4(0.2 + c * 2, 0.3 + c * 2, 0.5 + c, 1);
}
