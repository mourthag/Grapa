#version 430
in vec2 texCoord;
flat in int rotIndex;

out vec4 frag;

uniform sampler2D snowflakeTexture;

void main(void)
{
    vec4 color = texture(snowflakeTexture, texCoord).bgra;

    if(color.a == 0)
        discard;
    frag = color;
}
