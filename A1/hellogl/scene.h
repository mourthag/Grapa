#ifndef SCENE_H
#define SCENE_H

#include <QOpenGLFunctions_4_0_Core>
#include <QDebug>
#include <QOpenGLShaderProgram>

#include <openglmodel.h>
#include "tiny_gltf.h"

struct Material
{
    float diffuseFactor[4];
    int diffuseTexture;
    float specularFactor[3];
    int specularTexture;
    float shininessFactor;
    int shininessTexture;

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
        shininessFactor = 10;
        shininessTexture = 0;
    }
};

class Scene : QOpenGLFunctions_4_0_Core
{


public:

    Scene();
    void initGL();

    void loadFromGLTF(QOpenGLShaderProgram *prog, tinygltf::Model gltf_model);
    void drawScene(QOpenGLShaderProgram *prog, QMatrix4x4 *viewMat);

private:
    void loadTextures();
    void loadMaterials();
    void loadMeshes(QOpenGLShaderProgram *prog);

    tinygltf::Model model;

    GLuint textures;

    GLuint materialBuffer;
    std::vector<Material> materials;

    std::vector<OpenGLModel*> oglmodels;
};

#endif // SCENE_H
