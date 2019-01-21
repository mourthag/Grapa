#version 400
layout (location = 0) in vec2 pos;
layout (location = 1) in vec2 patchOffset;
out vec3 vPosition;
out int vPatchNumber;

uniform mat4 modelMat;
uniform vec3 camPos;
uniform float patchSize;
uniform int patchesPerRow;

void main(void)
{
    int patchX = clamp(int(patchOffset.x + int(camPos.x)/patchSize), 0, patchesPerRow * patchesPerRow );
    int patchZ = clamp(int(patchOffset.y + int(camPos.z)/patchSize), 0, patchesPerRow * patchesPerRow );
    vPatchNumber =  patchZ * patchesPerRow + patchX;


    vPosition = vec3(modelMat * vec4(pos.x, 0 , pos.y, 1.0));

}
