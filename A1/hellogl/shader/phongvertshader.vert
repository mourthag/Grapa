#version 400

in vec3 pos;
in vec3 fnormal;
in vec3 fcolor;

out vec3 normal;
out vec3 color;

uniform mat4 mvp;
uniform mat3 normalMat;

void main(void)
{
    vec4 wPos = mvp * vec4(pos, 1.0);
    vec3 norm = normalMat * fnormal;

    color = fcolor;
    normal = norm;
    gl_Position = wPos;
}
