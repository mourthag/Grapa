#ifndef SCENE_H
#define SCENE_H

#include <QOpenGLFunctions_4_0_Core>
#include <QDebug>
#include <QOpenGLShaderProgram>
#include <QTime>

#include <node.h>
#include <animation.h>
#include <openglmodel.h>
#include "tiny_gltf.h"
#include "terrain.h"

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

struct CameraLightInfo {
    QVector3D camTranslation;
    QMatrix4x4 camRotation;
    QMatrix4x4 projMatrix;
    QVector3D lightPos;
    float lightInt;

    QMatrix4x4 viewMatrix() {
        QMatrix4x4 translation;
        translation.translate(camTranslation);
        return camRotation * translation;
    }
};

class Scene : QOpenGLFunctions_4_0_Core
{


public:

    Scene();
    ~Scene();
    void initGL();
    void clear();

    void loadFromGLTF(tinygltf::Model gltf_model);
    void loadTerrain(QFile *pgmFile);
    void drawScene(QOpenGLShaderProgram *prog, bool setUpUniformBlocks);
    void drawTerrain(QOpenGLShaderProgram *prog);
    Node* findNode(int nodeIndex);
    CameraLightInfo *getCameraLightInfo();
    QVector3D getCamPos();
    void setUpUniforms(QOpenGLShaderProgram *prog, bool bufferUniformBlocks);

    void setAnimationPlay(bool play);

private:
    void loadTextures();
    void loadMaterials();
    void loadAnimations();
    void loadMeshes();
    void loadLightsBuffer();

    QTime timer;

    bool wasLoaded;
    bool animationPlaying;

    tinygltf::Model model;

    GLuint textures;

    CameraLightInfo camLightInfo;

    GLuint materialBuffer;
    std::vector<Material> materials;

    std::vector<Animation*> animations;
    std::vector<Node*> rootNodes;
    std::vector<Terrain*> terrains;
};

#endif // SCENE_H
