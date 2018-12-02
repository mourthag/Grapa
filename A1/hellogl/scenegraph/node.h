#ifndef NODE_H
#define NODE_H

#include <QMatrix4x4>
#include <QOpenGLShaderProgram>
#include <QDebug>

#include "openglmodel.h"
#include "tiny_gltf.h"


class Node
{
public:
    Node(tinygltf::Model *model, int nodeIndex);
    ~Node();

    void clear();
    void addModel(OpenGLModel *model);
    void loadMesh(tinygltf::Model *gltf_model, int meshIndex);
    void addChild(Node *child);
    void updateModelMatrix(QMatrix4x4 newMatrix);

    void draw(QOpenGLShaderProgram * prog, QMatrix4x4 viewMat);
    Node* findNode(int nodeIndex);
protected:
    void setParentNode(Node *parent);

    int name;

    QMatrix4x4 modelMatrix;
    QMatrix4x4 animationMatrix;
    QMatrix4x4 getTotalMatrix();
    Node *parentNode;

private:
    std::vector<OpenGLModel*> models;
    std::vector<Node*> children;
};

#endif // NODE_H
