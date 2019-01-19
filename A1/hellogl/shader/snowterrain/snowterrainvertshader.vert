#version 400

in vec2 pos;
in int patchOffset;
out vec3 vPosition;
out int vPatchNumber;

uniform mat4 modelMat;
uniform vec3 camPos;
uniform float patchSize;
uniform int patchesPerRow;

void main(void)
{
    vec2 basePatchPos = vec2(camPos.x, camPos.z)/patchSize;
    vPatchNumber = int(basePatchPos.x + patchesPerRow * basePatchPos.y + patchOffset);


    vPosition = vec3(modelMat * vec4(pos.x, 0 , pos.y, 1.0));

}
