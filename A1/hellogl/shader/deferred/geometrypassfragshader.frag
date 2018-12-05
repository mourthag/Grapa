#version 400
layout (location = 0) out vec4 NormTC;
layout (location = 1) out uvec4 material;

in vec2 texCoord;
in vec3 vNormal;

uniform int materialIndex;


void main(void)
{
    vec4 ntc = vec4(vNormal.x, vNormal.y, texCoord.x, texCoord.y);
    NormTC = ntc;
    material = uvec4(materialIndex, 0,0,0);
}
