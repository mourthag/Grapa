#ifndef SCENE_H
#define SCENE_H

#include <QDebug>
#include <QOpenGLShaderProgram>
#include <QTime>
#include <QObject>

#include "node.h"
#include "camera.h"
#include "light.h"
#include "material.h"
#include "animation.h"
#include "openglmodel.h"
#include "tiny_gltf.h"

class Scene : public QObject
{
public:

    Scene();
    ~Scene();
    void clear();

    void loadFromGLTF(tinygltf::Model gltf_model);
    void drawScene(QOpenGLShaderProgram *prog, bool setUpUniformBlocks);
    Node* findNode(int nodeIndex);
    Camera* getCamera();
    void setUpUniforms(QOpenGLShaderProgram *prog, bool bufferUniformBlocks);
    void setAnimationPlay(bool play);

    bool wasLoaded;
protected:
    Camera camera;

protected:
    void loadTextures();
    void loadMaterials();
    void loadAnimations();
    void loadMeshes();

    QTime timer;

    bool animationPlaying;

    tinygltf::Model model;

    GLuint textures;

    GLuint materialBuffer;
    std::vector<Material> materials;

    std::vector<Animation*> animations;
    std::vector<Node*> rootNodes;
};

#endif // SCENE_H
