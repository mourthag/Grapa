#version 430
in vec2 texCoord;

out vec4 frag;

void main(void)
{

    frag = vec4(texCoord,0,1);
}
