#version 400

struct Material
{
int diffuseTexture ;
int specularTexture ;
float shininessFactor ;
int shininessTexture ;
vec4 diffuseFactor;
vec3 specularFactor;
};

out vec4 frag;

in vec3 vLightPos;
in vec4 LightColor;

uniform mat4 v, p;

uniform sampler2D ntcTexture;
uniform usampler2D materialTexture;
uniform sampler2D depthTexture;

//0-standard phong
//1-normals
//2-UVs
//3-material
//4-viewSpacePos
uniform int renderMode;

layout (std140) uniform MaterialBlock {
    Material materials[256];
};

uniform sampler2DArray matTextures;

void main(void)
{
    vec2 pointCoord = 2 * gl_PointCoord - 1;
    if(length(pointCoord) > 1)
        discard;

    //Screenspace position
    vec2 scrPos = gl_FragCoord;

    vec4 ntcVal = texture(ntcTexture, scrPos);
    vec3 normal = vec3(ntcVal.x, ntcVal.y, sqrt(1-pow(ntcVal.x, 2)-pow(ntcVal.y,2)));

    //
    vec2 uv = ntcVal.zw;

    uint materialID = texture(materialTexture, scrPos).r;

    float depth = texture(depthTexture, scrPos).r;

    vec4 viewSpacePos = inverse(p) * vec4( 2 * scrPos - 1 ,  2 * depth -1 , 1.0);
    //normalized viewSpacePos
    vec3 pos = viewSpacePos.xyz  / viewSpacePos.w;

    if(renderMode == 0) {
        Material mat = materials[materialID];

        vec3 lightDir = normalize(vLightPos - pos);
        vec3 reflection = normalize(reflect(-lightDir,normal));
        vec3 viewDir = normalize(-pos);

        float fallOff = 1.0/ (pow(length(pos-vLightPos),2.0));

        vec4 kd = mat.diffuseFactor * texture(matTextures, vec3(uv, mat.diffuseTexture));
        vec4 dPart = kd * fallOff * LightColor * max(dot(normal, lightDir), 0.0);

        vec4 ka = 0.1 * kd;
        vec4 aPart = ka * LightColor * fallOff;

        vec4 ks = vec4(mat.specularFactor, 1.0) * texture(matTextures, vec3(uv, mat.specularTexture));
        float n = mat.shininessFactor * texture(matTextures, vec3(uv, mat.shininessTexture)).x;
        vec4 sPart = ks * fallOff * LightColor * pow(max(dot(reflection, viewDir), 0.0), n);

        vec4 col = aPart + dPart + sPart;
        frag = col;
    }
    else if(renderMode == 1) {
        frag = vec4(normal, 1.0);
    }
    else if(renderMode == 2) {
        frag = vec4(uv, 0, 1.0);
    }
    else if(renderMode == 3) {
        float mat = float(materialID);
        frag = vec4(mat/255.0, mat/255.0, mat/255.0, 1.0);
    }
    else if(renderMode == 4) {
        frag = vec4(pos, 1.0);
    }

}
