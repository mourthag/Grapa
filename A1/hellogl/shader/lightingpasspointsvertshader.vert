#version 400

//light data
layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 color;

out vec3 vLightPos;
out vec4 LightColor;

uniform mat4 m,v,p;

float max3 (vec3 v) {
  return max (max (v.x, v.y), v.z);
}

void main(void)
{

    vec4 worldPos = m * vec4(pos, 1.0);

    float d = sqrt(256 * max3(color));
    vec4 dProjected = p * vec4(d, 0, worldPos.z, 1);
    float dHomogen = dProjected.x / dProjected.w;

    LightColor = vec4(color, 1.0);
    vLightPos = vec3(v * worldPos);

    gl_PointSize = dHomogen;
    gl_Position = vec3(p * v * m * vec4(pos, 1.0));
}

