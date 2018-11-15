#version 400

in vec2 pos;

uniform usampler2D heightMap;

uniform mat4 m, v, p;

void main(void)
{
    float height = float(texture(heightMap, pos).r);

    vec3 position = vec3(pos.x, height, pos.y);
    gl_Position = p * vec4(position, 1.0);
}
