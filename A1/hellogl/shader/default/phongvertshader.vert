#version 400

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 vertnormal;
layout (location = 2) in vec2 UV;

out vec3 vPos;
out vec3 normal;
out vec2 texCoord;

uniform mat3 normalMat;
uniform mat4 modelMat, viewMat, projMat;

void main(void)
{
    vPos = vec3(viewMat * modelMat * vec4(pos, 1.0));
    vec3 norm = normalize(normalMat * vertnormal);

    normal = norm;
    texCoord = UV;
    gl_Position = projMat * vec4(vPos, 1.0);
}
