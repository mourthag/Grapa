#version 400

in vec2 texCoord;

out vec4 frag;

uniform sampler2D ntcTexture;
uniform sampler2D materialTexture;
uniform sampler2D depthTexture;

void main(void)
{
    frag = vec4(texCoord, 0, 0);
}
