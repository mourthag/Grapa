#version 400

layout (location = 0) in vec2 UV;

out vec2 texCoord;

void main(void)
{
    vec2 ndc = UV * 2 - vec2(1,1);
    texCoord = UV;
    gl_Position = vec4(ndc, 0.0, 1.0);
}
