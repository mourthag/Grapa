#version 400

out vec4 frag;

in vec3 normal;
in vec4 color;

void main(void)
{

    frag = vec4(normal, 1.0);
}
