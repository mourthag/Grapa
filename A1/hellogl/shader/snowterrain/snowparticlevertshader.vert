#version 430

layout (location = 0)in vec2 UV;
layout (location = 1) in vec4 particleData;

out vec2 texCoord;

uniform float radius;
uniform vec3 windDir;
uniform float fallingSpeed;
uniform float timeElapsed;

uniform mat4 viewMat, projMat;
uniform vec3 camPos;

const float DEG_TO_RAD = 3.141592653589793 / 180.0;

mat3 scale(float factor) {
    return mat3(
                factor, 0.0, 0.0,
                0.0, factor, 0.0,
                0.0, 0.0, factor );
}

mat4 rotateY(float rad) {
    float c = cos(rad);
    float s = sin(rad);
    return mat4(
          c, 0.0,  -s, 0.0,
        0.0, 1.0, 0.0, 0.0,
          s, 0.0,   c, 0.0,
        0.0, 0.0, 0.0, 1.0
    );
}

mat4 rotateZ(float rad) {
    float c = cos(rad);
    float s = sin(rad);
    return mat4(
        1.0, 0.0, 0.0, 0.0,
        0.0,   c,  -s, 0.0,
        0.0,   s,   c, 0.0,
        0.0, 0.0, 0.0, 1.0
    );
}

float setInCameraRange(float coord, float camCoord) {
    float result = coord;


    while(result > camCoord + radius) {
        result -= 2.0 * radius;
    }

    while(result < camCoord - radius ) {
        result += 2.0 * radius;
    }

    return result;
}

void main(void)
{

    mat4 rotationMat = rotateY(particleData.w);

    vec3 wPos = particleData.xyz + scale(0.01) * vec3(UV.x - 0.5,  UV.y - 0.5, 0.0);
    vec3 rotWPos = vec3(rotationMat * vec4(wPos ,1.0));

    vec3 opWPos = particleData.xyz;
    vec3 rotOpWPos = vec3(rotationMat * vec4(opWPos ,1.0));

    rotWPos += windDir * (sin(timeElapsed)  + 1.2 + timeElapsed) - timeElapsed * vec3(0, fallingSpeed, 0);
    rotOpWPos += windDir * (sin(timeElapsed)  + 1.2 + timeElapsed) - timeElapsed * vec3(0, fallingSpeed, 0);

    vec3 offset = rotWPos - rotOpWPos;

    rotOpWPos.x = setInCameraRange(rotOpWPos.x, camPos.x);
    rotOpWPos.y = setInCameraRange(rotOpWPos.y, camPos.y);
    rotOpWPos.z = setInCameraRange(rotOpWPos.z, camPos.z);

    rotWPos = rotOpWPos + offset;

    vec3 vPos = vec3(viewMat * vec4(rotWPos, 1.0));
    texCoord = UV;
    gl_Position = projMat * vec4(vPos, 1.0);
}
