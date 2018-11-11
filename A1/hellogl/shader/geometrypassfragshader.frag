#version 400
layout (location = 0) out vec4 NormTC;
layout (location = 1) out uint material;

in vec2 texCoord;
in vec3 vNormal;

uniform uint materialID;


void main(void)
{
    vec4 ntc = vec4(texCoord, vNormal.x, vNormal.y);
    NormTC = ntc;
    material = materialID;
}
