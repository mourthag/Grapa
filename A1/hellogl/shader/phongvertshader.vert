#version 400

in vec3 pos;
in vec3 vertnormal;
in vec2 UV;

out vec3 vPos;
out vec3 normal;
out vec2 texCoord;

uniform mat3 normalMat;
uniform mat4 m,v,p;

void main(void)
{
    vPos = vec3(v * m * vec4(pos, 1.0));
    vec3 norm = normalize(normalMat * vertnormal);

    normal = norm;
    texCoord = UV;
    gl_Position = p * vec4(vPos, 1.0);
}
