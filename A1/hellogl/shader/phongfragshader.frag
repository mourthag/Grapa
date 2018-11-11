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

out vec4 frag;

in vec3 vPos;
in vec3 normal;
in vec2 texCoord;

uniform mat4 m,v,p;
uniform vec3 lightPos;
uniform float lightInt;

uniform int materialIndex;

layout (std140) uniform MaterialBlock {
    Material materials[256];
};
uniform sampler2DArray matTextures;

void main(void)
{

    Material mat = materials[materialIndex];

    vec3 vLightPos = vec3( v * m * vec4(lightPos, 1.0));
    vec3 lightDir = normalize(vLightPos - vPos);
    vec3 reflection = reflect(-lightDir,normal);
    vec3 viewDir = normalize(-vPos);

    vec4 kd = mat.diffuseFactor * texture(matTextures, vec3(texCoord,mat.diffuseTexture));
    vec4 dPart = kd * lightInt * max(dot(normal, lightDir), 0.0);

    vec4 ka = 0.1 * kd;
    vec4 aPart = ka * lightInt;

    vec4 ks = vec4(mat.specularFactor,1.0) * texture(matTextures, vec3(texCoord, mat.specularTexture));
    float n = mat.shininessFactor * texture(matTextures, vec3(texCoord, mat.shininessTexture)).x;
    vec4 sPart = ks * lightInt * pow(max(dot(reflection, viewDir), 0.0), n);

    vec4 col = aPart + dPart + sPart;
    frag = col;
}
