#version 400

in vec3 pos;
in vec3 fnormal;
in vec3 fcolor;

out vec3 normal;
out vec3 color;

uniform mat4 mv, p;
uniform mat3 normalMat;

uniform vec3 lightPos;
uniform float lightInt;
uniform int n;

void main(void)
{
    vec4 vPos =  mv * vec4(pos, 1.0);
    vec3 norm = normalize(normalMat * fnormal);

    vec3 kd = fcolor;
    vec3 ka = 0.1 * kd;

    vec3 lightDir = normalize(lightPos - vPos.xyz);
    vec3 r = reflect(-lightDir,norm);
    vec3 v = normalize(-vPos.xyz);

    vec3 dPart = kd * lightInt * max(dot(norm, lightDir), 0.0);
    vec3 sPart = vec3(1,1,1) * lightInt * pow(max(dot(r, v), 0.0), n);

    color = ka * lightInt + dPart + sPart;
    gl_Position = p * mv * vec4(pos,1.0);
    normal = norm;
}
