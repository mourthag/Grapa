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
