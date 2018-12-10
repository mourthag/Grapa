#version 430
in vec2 texCoord;
flat in int rotIndex;

out vec4 frag;

uniform sampler2DArray impostorTextures;

void main(void)
{
    vec4 color = texture(impostorTextures, vec3(texCoord, rotIndex));

    if(color.a == 0)
        discard;
    frag = color;
}
