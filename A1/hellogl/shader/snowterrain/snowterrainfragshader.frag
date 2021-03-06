#version 400
in vec3 tePosition;
in vec3 teNormal;
in vec2 UV;
in vec2 patchUV;
in float snowHeight;
flat in int patchNumber;

out vec4 frag;

uniform mat4 modelMat, viewMat, projMat;
uniform mat3 normalMat;
uniform vec3 lightPos;
uniform vec3 lightColor;
uniform float lightInt;

uniform vec3 rockSpecular;
uniform float rockShininess;
uniform float rockSlope;
uniform float rockMargin;

uniform vec3 gravelSpecular;
uniform float gravelShininess;
uniform float gravelHeight;
uniform float gravelMargin;

uniform vec3 sandSpecular;
uniform float sandShininess;
uniform float sandHeight;
uniform float sandMargin;

uniform vec3 stoneSpecular;
uniform float stoneShininess;
uniform float stoneSlope;
uniform float stoneMargin;

uniform vec3 snowSpecular;
uniform float snowShininess;

uniform float heightScaling;

//0-rockMaterial
//1-gravelMaterial
//2-sandMaterial
//3-stoneMaterial
//4-snowMaterial
uniform sampler2DArray materialTextures;

uniform sampler2DArray snowMaps;
uniform int patchesPerRow;


void main(void)
{

    vec4 rockDiffuse = texture(materialTextures, vec3(UV, 0)).bgra;
    vec4 gravelDiffuse = texture(materialTextures, vec3(UV, 1)).bgra;
    vec4 sandDiffuse = texture(materialTextures, vec3(UV, 2)).bgra;
    vec4 stoneDiffuse = texture(materialTextures, vec3(UV, 3)).bgra;
    vec4 snowDiffuse = texture(materialTextures, vec3(UV, 4)).bgra;

    float snowInterp = min(snowHeight , 1.0);

    float heightInterpolation = smoothstep(sandHeight, sandHeight + sandMargin, tePosition.y);
    float rockStoneInterpolation = smoothstep(rockSlope, rockSlope + rockMargin, abs(teNormal.y));
    float stoneDirtInterpolation = smoothstep(stoneSlope, stoneSlope + stoneMargin, abs(teNormal.y));

    vec4 heightMixedDiffuse = mix(sandDiffuse, gravelDiffuse, heightInterpolation);
    vec4 rockStoneMixedDiffuse = mix(rockDiffuse, stoneDiffuse, rockStoneInterpolation);
    vec4 slopeMixedDiffuse = mix(rockStoneMixedDiffuse, heightMixedDiffuse, stoneDirtInterpolation);
    vec4 snowMixedDiffuse = slopeMixedDiffuse * (1-snowInterp) + snowDiffuse * snowInterp;

    float heightMixedShininess = mix(sandShininess, gravelShininess, heightInterpolation);
    float slopeMixedShininess = mix(rockShininess, heightMixedShininess, stoneDirtInterpolation);
    float snowMixedShininess = slopeMixedShininess * (1- snowInterp) + snowShininess * snowInterp;

    vec3 heightMixedSpecular = mix(sandSpecular, gravelSpecular, heightInterpolation);
    vec3 slopeMixedSpecular = mix(rockSpecular, heightMixedSpecular, stoneDirtInterpolation);
    vec3 snowMixedSpecular = slopeMixedSpecular * (1-snowInterp) + snowSpecular * snowInterp;

    vec3 normal = normalMat * teNormal;
    vec3 vPos = vec3(viewMat * vec4(tePosition, 1.0));
    vec3 vLightPos = vec3( viewMat * vec4(lightPos, 1.0));
    vec3 lightDir = normalize(vLightPos - vPos);
    vec3 reflection = reflect(-lightDir,normal);
    vec3 viewDir = normalize(-vPos);

    float fallOff = 1.0/ (pow(length(vPos-vLightPos),0.2));
    vec4 light = vec4(lightColor,1.0) * lightInt * fallOff;

    vec4 kd =  0.5 * snowMixedDiffuse;
    vec4 dPart = kd * light * max(dot(normal, lightDir), 0.0);

    vec4 ka = 0.5 * snowMixedDiffuse;
    vec4 aPart = ka * light;

    vec4 ks = vec4(snowMixedSpecular,1.0);
    float n = snowMixedShininess;
    vec4 sPart = ks * light * pow(max(dot(reflection, viewDir), 0.0), n);


    vec4 col = aPart + dPart + sPart ;
    frag = col;
}
