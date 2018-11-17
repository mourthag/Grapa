#version 400

in vec2 pos;
out vec3 vPosition;

uniform usampler2D heightMap;


void main(void)
{
    float height = float(texture( heightMap, (pos+vec2(50, 50))/100.0).r) / 1000;
    vPosition = vec3(pos.x, height , pos.y);

}
