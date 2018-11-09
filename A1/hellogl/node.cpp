#include "node.h"

Node::Node(QOpenGLShaderProgram *prog, tinygltf::Model *model, int nodeIndex): parentNode(NULL) {


    QMatrix4x4 matrix;
    matrix.setToIdentity();
    tinygltf::Node gltf_node = model->nodes[nodeIndex];
    name = nodeIndex;

    if(gltf_node.matrix.size() > 0) {
        matrix = QMatrix4x4(reinterpret_cast<float*>(&gltf_node.matrix.at(0)));
    }
    else {
        QMatrix4x4 scaleMat;
        if(gltf_node.scale.size() > 0) {
            scaleMat.scale(gltf_node.scale[0],gltf_node.scale[1],gltf_node.scale[2]);
        }
        QMatrix4x4 rotMat;
        if(gltf_node.rotation.size() > 0) {
            QQuaternion quat = QQuaternion(gltf_node.rotation[0], gltf_node.rotation[1], gltf_node.rotation[2], gltf_node.rotation[3]);
            rotMat.rotate(quat);
        }
        QMatrix4x4 transMat;
        if(gltf_node.translation.size() > 0) {
            transMat.translate(gltf_node.translation[0], gltf_node.translation[1], gltf_node.translation[2]);
        }
        matrix = transMat * rotMat * scaleMat;
    }

    modelMatrix = matrix;

    if(gltf_node.mesh > -1)
        loadMesh(prog, model, gltf_node.mesh);

    for(int childIndex = 0; childIndex < gltf_node.children.size(); childIndex++) {
        int childNodeIndex = gltf_node.children[childIndex];

        Node* child = (Node*)malloc(sizeof(Node));
        child = new Node(prog, model, childNodeIndex);
        this->addChild(child);
    }

}

Node::~Node() {
}

void Node::setParentNode(Node *parent) {
    parentNode = parent;
}

void Node::clear() {
    for(int i = 0; i < models.size(); i++) {
        models[i]->clear();
        delete(models[i]);
    }
    models.clear();
    for(int j = 0; j < children.size();j++) {
        children[j]->clear();
        delete(children[j]);
    }
    children.clear();

}

void Node::loadMesh(QOpenGLShaderProgram *prog, tinygltf::Model *gltf_model, int meshIndex) {
    tinygltf::Mesh *mesh = &gltf_model->meshes[meshIndex];
    for(int i = 0; i < mesh->primitives.size(); i++) {

        OpenGLModel *oglModel = (OpenGLModel*)malloc(sizeof(OpenGLModel));
        oglModel = new OpenGLModel();
        oglModel->loadGLTF(prog, gltf_model, meshIndex, i);
        addModel(oglModel);
    }

}

void Node::addModel(OpenGLModel *model) {
    models.push_back(model);
}

void Node::addChild(Node *child) {
    children.push_back(child);
    child->setParentNode(this);
}

QMatrix4x4 Node::getTotalMatrix() {
    if(parentNode) {
        return parentNode->getTotalMatrix() * modelMatrix;
    }
    else
        return modelMatrix;
}

void Node::draw(QOpenGLShaderProgram *prog, QMatrix4x4 *viewMat) {

    for(int modelIndex = 0; modelIndex < models.size(); modelIndex++) {

        QMatrix4x4 mat = getTotalMatrix();
        prog->setUniformValue("m", mat);
        prog->setUniformValue("normalMat", (*viewMat * mat).normalMatrix());
        models[modelIndex]->drawModel(prog);
    }
    for(int childIndex = 0; childIndex < children.size(); childIndex++) {
        children[childIndex]->draw(prog, viewMat);
    }

}
