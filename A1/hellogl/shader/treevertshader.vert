#version 400

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 vertnormal;
layout (location = 2) in vec2 UV;
layout (location = 4) in vec4 treeData;

out vec3 vPos;
out vec3 normal;
out vec2 texCoord;

uniform usampler2D heightMap;
uniform float heightScaling;

uniform mat3 normalMat;
uniform mat4 modelMat, viewMat, projMat;

void main(void)
{
    vec2 gridPos = treeData.xy/2000.0;
    float height = float(textureLod( heightMap, gridPos,  0).r) * heightScaling;

    vec3 wPos = pos + vec3(0,1,0) * height;

    vPos = vec3(viewMat * modelMat * vec4(wPos, 1.0));
    vec3 norm = normalize(normalMat * vertnormal);

    normal = norm;
    texCoord = UV;
    gl_Position = projMat * vec4(vPos, 1.0);
}
