#version 430

in vec2 UV;
layout (location = 4) in vec4 treeData;

out vec4 vTreeData;
out vec2 texCoord;

uniform mat4 viewMat, projMat;
uniform usampler2D heightMap;
uniform vec3 camPos;
uniform float heightScaling;

const float DEG_TO_RAD = 3.141592653589793 / 180.0;

mat4 rotateY(float rad) {
    float c = cos(rad);
    float s = sin(rad);
    return mat4(
        c, 0.0, -s, 0.0,
        0.0, 1.0, 0.0, 0.0,
        s, 0.0, c, 0.0,
        0.0, 0.0, 0.0, 1.0
    );
}

mat4 scale(float factor) {
    return mat4(
                factor, 0.0, 0.0, 0.0,
                0.0, factor, 0.0, 0.0,
                0.0, 0.0, factor, 0.0,
                0.0, 0.0, 0.0, 1.0);
}

void main(void)
{
    vec2 gridPos = treeData.xy/4096.0;

    vec3 normal = vec3( 0,0,-1);
    vec3 newNormal = normalize(vec3(camPos - vec3(treeData.x, 0, treeData.y)));
    float angle = atan(newNormal.x, newNormal.z) - atan(normal.x , normal.z);

    mat4 rotationMat = rotateY(angle);
    mat4 scaleMat = scale(treeData.z);
    float height = float(texture( heightMap, gridPos).r) * heightScaling;

    vec4 wPosOffset = vec4(treeData.x, height , treeData.y, 0.0);

    vec3 vPos = vec3(viewMat * (rotationMat * scaleMat * vec4(UV - vec2(0.5,0), 0, 1.0) + wPosOffset));

    vTreeData = treeData;
    texCoord = UV;
    gl_Position = projMat * vec4(vPos, 1.0);
}
