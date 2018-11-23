#version 400
in vec3 tePosition;
out vec4 frag;

void main(void)
{
    float gray = tePosition.y / 30.0;
    frag = vec4(gray, gray, gray, 1.0);
}
