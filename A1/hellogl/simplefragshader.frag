#version 400

out vec4 frag;

in vec3 normal;
in vec3 color;

void main(void)
{
    frag = vec4(color, 1.0);
}
