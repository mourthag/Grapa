#include "node.h"

Node::Node(QMatrix4x4 matrix)
{
    Node(matrix, matrix);
}

Node::Node(QMatrix4x4 matrix, QMatrix4x4 parentMatrix) {
    modelMatrix = matrix;
    totalMatrix = parentMatrix * matrix;
}

Node::~Node() {
}

void Node::clear() {
    for(int i = 0; i < meshes.size(); i++) {
        meshes[i]->clear();
        delete(meshes[i]);
    }
    meshes.clear();
    for(int j = 0; j < childs.size();j++) {
        childs[j]->clear();
        delete(childs[j]);
    }
    childs.clear();

}

void Node::addMesh(OpenGLModel *mesh) {
    meshes.push_back(mesh);
}

Node* Node::addChild(QMatrix4x4 childMatrix) {
    Node *child = (Node*)malloc(sizeof(Node));
    child = new Node(childMatrix, totalMatrix);
    childs.push_back(child);
    return child;
}
