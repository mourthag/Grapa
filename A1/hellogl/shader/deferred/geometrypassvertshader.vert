#version 400

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 vertnormal;
layout (location = 2) in vec2 UV;

out vec2 texCoord;
out vec3 vNormal;

uniform mat4 modelMat, viewMat, projMat;
uniform mat3 normalMat;

void main(void)
{
    vec3 vPos = vec3(viewMat * modelMat * vec4(pos, 1.0));
    vec3 norm = normalize(normalMat * vertnormal);

    vNormal = norm;
    texCoord = UV;
    gl_Position = projMat * vec4(vPos, 1.0);

}
