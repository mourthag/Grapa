#version 400

out vec4 frag;

in vec3 vPos;
in vec3 normal;
in vec3 color;

uniform vec3 lightPos;
uniform float lightInt;
uniform int n;

void main(void)
{
    vec3 kd = color;
    vec3 ka = 0.1 * kd;

    vec3 lightDir = normalize(lightPos - vPos);
    vec3 r = reflect(-lightDir,normal);
    vec3 v = normalize(-vPos);

    vec3 dPart = kd * lightInt * max(dot(normal, lightDir), 0.0);
    vec3 sPart = vec3(1,1,1) * lightInt * pow(max(dot(r, v), 0.0), n);

    vec3 col = ka * lightInt + dPart +sPart;

    frag = vec4(col, 1.0);
}
