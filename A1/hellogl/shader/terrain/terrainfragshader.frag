#version 400
in vec3 tePosition;
in vec3 teNormal;
in vec2 UV;

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

uniform float heightScaling;

//0-rockMaterial
//1-gravelMaterial
//2-sandMaterial
//3-stoneMaterial
uniform sampler2DArray materialTextures;

void main(void)
{

    vec4 rockDiffuse = texture(materialTextures, vec3(UV, 0)).bgra;
    vec4 gravelDiffuse = texture(materialTextures, vec3(UV, 1)).bgra;
    vec4 sandDiffuse = texture(materialTextures, vec3(UV, 2)).bgra;
    vec4 stoneDiffuse = texture(materialTextures, vec3(UV, 3)).bgra;

    float heightInterpolation = smoothstep(sandHeight, sandHeight + sandMargin, tePosition.y);
    float rockStoneInterpolation = smoothstep(rockSlope, rockSlope + rockMargin, abs(teNormal.y));
    float stoneDirtInterpolation = smoothstep(stoneSlope, stoneSlope + stoneMargin, abs(teNormal.y));

    vec4 heightMixedDiffuse = mix(sandDiffuse, gravelDiffuse, heightInterpolation);
    vec4 rockStoneMixedDiffuse = mix(rockDiffuse, stoneDiffuse, rockStoneInterpolation);
    vec4 slopeMixedDiffuse = mix(rockStoneMixedDiffuse, heightMixedDiffuse, stoneDirtInterpolation);

    float heightMixedShininess = mix(sandShininess, gravelShininess, heightInterpolation);
    float slopeMixedShininess = mix(rockShininess, heightMixedShininess, stoneDirtInterpolation);

    vec3 heightMixedSpecular = mix(sandSpecular, gravelSpecular, heightInterpolation);
    vec3 slopeMixedSpecular = mix(rockSpecular, heightMixedSpecular, stoneDirtInterpolation);

    vec3 normal = normalMat * teNormal;
    vec3 vPos = vec3(viewMat * vec4(tePosition, 1.0));
    vec3 vLightPos = vec3( viewMat * vec4(lightPos, 1.0));
    vec3 lightDir = normalize(vLightPos - vPos);
    vec3 reflection = reflect(-lightDir,normal);
    vec3 viewDir = normalize(-vPos);

    float fallOff = 1.0/ (pow(length(vPos-vLightPos),0.2));
    vec4 light = vec4(lightColor,1.0) * lightInt * fallOff;

    vec4 kd =  1.0 * slopeMixedDiffuse;
    vec4 dPart = kd * light * max(dot(normal, lightDir), 0.0);

    vec4 ka = 0.1 * slopeMixedDiffuse;
    vec4 aPart = ka * light;

    vec4 ks = vec4(slopeMixedSpecular,1.0);
    float n = slopeMixedShininess;
    vec4 sPart = ks * light * pow(max(dot(reflection, viewDir), 0.0), n);

    vec4 col = aPart + dPart + sPart;
    frag = col;
}
