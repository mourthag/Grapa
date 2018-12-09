#version 400

out vec4 frag;

in vec3 vPos;

uniform samplerCube skyboxTexture;

void main(void)
{
    frag = texture(skyboxTexture, vPos).bgra;
}
