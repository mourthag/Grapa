#version 400

in vec3 pos;
in vec3 fnormal;
in vec3 fcolor;

out vec3 normal;
out vec3 color;

uniform mat4 mvp;
//face color and normal

void main(void)
{
    normal = fnormal;
    color = fcolor;
    gl_Position = mvp * vec4(pos, 1.0);
}
