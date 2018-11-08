#ifndef NODE_H
#define NODE_H

#include <QMatrix4x4>
#include <QOpenGLShaderProgram>
#include <openglmodel.h>

class Node
{
public:
    Node(QMatrix4x4 matrix);
    Node(QMatrix4x4 matrix, QMatrix4x4 parentMatrix);
    ~Node();

    void clear();
    void addMesh(OpenGLModel *mesh);
    Node* addChild(QMatrix4x4 childMatrix);


    void draw(QOpenGLShaderProgram * prog);
protected:
    QMatrix4x4 modelMatrix;
    QMatrix4x4 totalMatrix;

private:
    std::vector<OpenGLModel*> meshes;
    std::vector<Node*> childs;
};

#endif // NODE_H
