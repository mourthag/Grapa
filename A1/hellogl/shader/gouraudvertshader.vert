#version 400

struct Material
{
vec4 diffuseFactor;
int diffuseTexture ;
vec3 specularFactor;
int specularTexture ;
float shininessFactor ;
int shininessTexture ;
};

in vec3 pos;
in vec3 vertnormal;
in vec2 UV;

out vec3 normal;
out vec4 color;

uniform mat4 m, v, p;
uniform mat3 normalMat;
uniform int materialIndex;

layout (std140) uniform MaterialBlock {
    Material materials[256];
};
uniform sampler2DArray matTextures;

uniform vec3 lightPos;
uniform float lightInt;

void main(void)
{
    vec4 vPos =  v * m * vec4(pos, 1.0);
    vec3 norm = normalize(normalMat * vertnormal);

    Material mat = materials[materialIndex];

    vec3 vLightPos = vec3(v * m * vec4(lightPos, 1.0));
    vec3 lightDir = normalize(vLightPos - vPos.xyz);
    vec3 reflection = reflect(-lightDir,norm);
    vec3 viewDir = normalize(-vPos.xyz);

    vec4 kd = mat.diffuseFactor * texture(matTextures, vec3(UV,mat.diffuseTexture));
    vec4 dPart = kd * lightInt * max(dot(norm, lightDir), 0.0);

    vec4 ka = 0.1 * kd;
    vec4 aPart = ka * lightInt;

    vec4 ks = vec4(mat.specularFactor,1.0) * texture(matTextures, vec3(UV, mat.specularTexture));
    float n = mat.shininessFactor * texture(matTextures, vec3(UV, mat.shininessTexture)).x;
    vec4 sPart = ks * lightInt * pow(max(dot(reflection, viewDir), 0.0), n);

    color = aPart + dPart + sPart;
    gl_Position = p * vPos;
    normal = norm;
}
