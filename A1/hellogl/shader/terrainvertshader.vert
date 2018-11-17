#version 400

in vec2 pos;
out vec3 vPosition;


void main(void)
{
    vPosition = vec3(pos.x, 0 , pos.y);

}
