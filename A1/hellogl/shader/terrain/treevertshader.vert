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
    mat4 rotationMat = rotateY(treeData.w * DEG_TO_RAD);
    mat4 scaleMat = scale(treeData.z);

    vec2 gridPos = treeData.xy/2000.0;
    float height = float(texture( heightMap, gridPos).r) * heightScaling;

    vec4 wPosOffset = vec4(treeData.x, height , treeData.y, 0.0);

    vPos = vec3(viewMat * (rotationMat * scaleMat * modelMat * vec4(pos, 1.0) + wPosOffset));
    vec3 norm = normalize(normalMat * vertnormal);

    normal = norm;
    texCoord = UV;
    gl_Position = projMat * vec4(vPos, 1.0);
}
