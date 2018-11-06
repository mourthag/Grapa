#version 400

in vec3 pos;
in vec3 fnormal;
in vec3 fcolor;

out vec3 normal;
out vec3 color;

uniform mat4 m, v, p;
uniform mat3 normalMat;

uniform vec3 lightPos;
uniform float lightInt;
uniform int n;

void main(void)
{
    vec4 vPos =  v * m * vec4(pos, 1.0);
    vec3 norm = normalize(normalMat * fnormal);

    vec3 kd = fcolor;
    vec3 ka = 0.1 * kd;

    vec3 vLightPos = vec3(p * v * m * vec4(lightPos, 1.0));
    vec3 lightDir = normalize(vLightPos - vPos.xyz);
    vec3 reflection = reflect(-lightDir,norm);
    vec3 viewDir = normalize(-vPos.xyz);

    vec3 dPart = kd * lightInt * max(dot(norm, lightDir), 0.0);
    vec3 sPart = vec3(1,1,1) * lightInt * pow(max(dot(reflection, viewDir), 0.0), n);

    color = ka * lightInt + dPart + sPart;
    gl_Position = p * v * m * vec4(pos,1.0);
    normal = norm;
}
