#ifndef MATERIAL_H
#define MATERIAL_H



struct Material
{
    int diffuseTexture;
    int specularTexture;
    float shininessFactor;
    int shininessTexture;
    float diffuseFactor[4];
    float specularFactor[3];
    float pad;

    Material() {
        diffuseFactor[0] = 1;
        diffuseFactor[1] = 1;
        diffuseFactor[2] = 1;
        diffuseFactor[3] = 1;
        diffuseTexture = 0;
        specularFactor[0] = 1;
        specularFactor[1] = 1;
        specularFactor[2] = 1;
        specularTexture = 0;
        shininessFactor = 100;
        shininessTexture = 0;
    }
};


#endif // MATERIAL_H
