#version 400

out vec4 frag;

in vec3 vPos;
in vec3 normal;
in vec3 color;

uniform mat4 m,v,p;
uniform vec3 lightPos;
uniform float lightInt;
uniform int n;

void main(void)
{
    vec3 kd = color;
    vec3 ka = 0.1 * kd;

    vec3 vLightPos = vec3(p * v * m * vec4(lightPos, 1.0));
    vec3 lightDir = normalize(vLightPos - vPos);
    vec3 reflection = reflect(-lightDir,normal);
    vec3 viewDir = normalize(-vPos);

    vec3 dPart = kd * lightInt * max(dot(normal, lightDir), 0.0);
    vec3 sPart = vec3(1,1,1) * lightInt * pow(max(dot(reflection, viewDir), 0.0), n);

    vec3 col = ka * lightInt + dPart +sPart;

    frag = vec4(col, 1.0);
}
