#version 400

out vec4 frag;

uniform vec3 color;

void main(void)
{
    frag = vec4(color, 1.0);
}
