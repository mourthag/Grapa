#version 400

in vec2 pos;
out vec3 vPosition;

uniform mat4 modelMat;

void main(void)
{
    vPosition = vec3(modelMat * vec4(pos.x, 0 , pos.y, 1.0));

}
