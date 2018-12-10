#version 430
in vec2 texCoord;

out vec4 frag;

uniform sampler2DArray impostorTextures;

void main(void)
{
    vec4 color = texture(impostorTextures, vec3(texCoord,0));

    if(color.a == 0)
        discard;
    frag = color;
}
