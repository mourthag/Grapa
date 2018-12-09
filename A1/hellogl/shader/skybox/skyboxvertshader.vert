#version 400

layout (location=0) in vec3 pos;

out vec3 vPos;

uniform mat4 modelMat, viewMat, projMat;

void main(void)
{
    vPos = pos;
    vec4 mvpPos = projMat * viewMat * modelMat * vec4(pos ,1.0);
    mvpPos.w = mvpPos.z;
    gl_Position = mvpPos;
}
