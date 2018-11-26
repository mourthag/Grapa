#version 400
in vec3 tePosition;
in vec3 teNormal;
in vec2 UV;

out vec4 frag;

uniform mat4 modelMat, viewMat, projMat;
uniform vec3 lightPos;
uniform float lightInt;

uniform vec3 rockSpecular;
uniform float rockShininess;
uniform float rockSlope;
uniform float rockMargin;

uniform vec3 gravelSpecular;
uniform float gravelShininess;

uniform vec3 sandSpecular;
uniform float sandShininess;
uniform float sandHeight;
uniform float sandMargin;

//0-rockMaterial
//1-gravelMaterial
//2-sandMaterial
uniform sampler2DArray materialTextures;

void main(void)
{

    vec4 rockDiffuse = texture(materialTextures, vec3(UV, 0)).bgra;
    vec4 gravelDiffuse = texture(materialTextures, vec3(UV, 1)).bgra;
    vec4 sandDiffuse = texture(materialTextures, vec3(UV, 2)).bgra;

    float heightInterpolation = smoothstep(sandHeight, sandHeight + sandMargin, tePosition.y);
    float normalInterpolation = smoothstep(rockSlope, rockSlope + rockMargin, abs(teNormal.y));

    vec4 heightMixedDiffuse = mix(sandDiffuse, gravelDiffuse, heightInterpolation);
    vec4 slopeMixedDiffuse = mix(rockDiffuse, heightMixedDiffuse, normalInterpolation);

    float heightMixedShininess = mix(sandShininess, gravelShininess, heightInterpolation);
    float slopeMixedShininess = mix(rockShininess, heightMixedShininess, normalInterpolation);

    vec3 heightMixedSpecular = mix(sandSpecular, gravelSpecular, heightInterpolation);
    vec3 slopeMixedSpecular = mix(rockSpecular, heightMixedSpecular, normalInterpolation);

    vec3 vPos = vec3(viewMat * vec4(tePosition, 1.0));
    vec3 vLightPos = vec3( viewMat * vec4(lightPos, 1.0));
    vec3 lightDir = normalize(vLightPos - vPos);
    vec3 reflection = reflect(-lightDir,teNormal);
    vec3 viewDir = normalize(-vPos);

    float fallOff = 1.0;/// (pow(length(vPos-vLightPos),2.0));

    vec4 kd = slopeMixedDiffuse;
    vec4 dPart = kd * fallOff * lightInt * max(dot(teNormal, lightDir), 0.0);

    vec4 ka = 0.1 * kd;
    vec4 aPart = ka * fallOff * lightInt;

    vec4 ks = vec4(slopeMixedSpecular,1.0);
    float n = slopeMixedShininess;
    vec4 sPart = ks * fallOff * lightInt * pow(max(dot(reflection, viewDir), 0.0), n);

    vec4 col = aPart + dPart + sPart;
    frag = slopeMixedDiffuse;
}
