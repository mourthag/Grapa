#version 400

in vec3 pos;
uniform mat4 mvp;

void main(void)
{
    gl_Position = mvp * vec4(pos, 1.0);
}
