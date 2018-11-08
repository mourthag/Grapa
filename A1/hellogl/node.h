#ifndef NODE_H
#define NODE_H

#include <QMatrix4x4>
#include <QOpenGLShaderProgram>
#include <openglmodel.h>
#include <QDebug>

#include "tiny_gltf.h"

class Node
{
public:
    Node(QOpenGLShaderProgram *prog, tinygltf::Model *model, int nodeIndex);
    Node(QOpenGLShaderProgram *prog, tinygltf::Model *model, int nodeIndex, Node *parent);
    ~Node();

    void clear();
    void addModel(OpenGLModel *model);
    void loadMesh(QOpenGLShaderProgram *prog, tinygltf::Model *gltf_model, int meshIndex);
    void addChild(Node *child);

    void draw(QOpenGLShaderProgram * prog, QMatrix4x4 *viewMat);
protected:
    void setParentNode(Node *parent);

    std::string name;

    QMatrix4x4 modelMatrix;
    QMatrix4x4 getTotalMatrix();
    Node *parentNode;

private:
    std::vector<OpenGLModel*> models;
    std::vector<Node*> children;
};

#endif // NODE_H
