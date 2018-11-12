#include "node.h"

Node::Node(tinygltf::Model *model, int nodeIndex): parentNode(NULL) {

    animationMatrix.setToIdentity();

    QMatrix4x4 matrix;
    matrix.setToIdentity();

    tinygltf::Node gltf_node = model->nodes[nodeIndex];
    name = nodeIndex;

    if(gltf_node.matrix.size() > 0) {

        for(int column=0; column < 4; column++) {
            QVector4D col;

            col.setX(gltf_node.matrix[column * 4 + 0]);
            col.setY(gltf_node.matrix[column * 4 + 1]);
            col.setZ(gltf_node.matrix[column * 4 + 2]);
            col.setW(gltf_node.matrix[column * 4 + 3]);
            matrix.setColumn(column, col);
        }
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
        loadMesh(model, gltf_node.mesh);

    for(int childIndex = 0; childIndex < gltf_node.children.size(); childIndex++) {
        int childNodeIndex = gltf_node.children[childIndex];

        Node* child = (Node*)malloc(sizeof(Node));
        child = new Node(model, childNodeIndex);
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

void Node::loadMesh(tinygltf::Model *gltf_model, int meshIndex) {
    tinygltf::Mesh *mesh = &gltf_model->meshes[meshIndex];
    for(int i = 0; i < mesh->primitives.size(); i++) {

        OpenGLModel *oglModel = (OpenGLModel*)malloc(sizeof(OpenGLModel));
        oglModel = new OpenGLModel();
        oglModel->loadGLTF(gltf_model, meshIndex, i);
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

void Node::updateModelMatrix(QMatrix4x4 newMatrix) {
    animationMatrix = newMatrix;
}

Node* Node::findNode(int nodeIndex) {
    if(name == nodeIndex)
        return this;
    else {
        Node* nodePtr = NULL;
        for(int i = 0; i< children.size(); i++) {
            if(!nodePtr)
                nodePtr = children[i]->findNode(nodeIndex);
        }
        return nodePtr;
    }
}

QMatrix4x4 Node::getTotalMatrix() {
    if(parentNode) {
        return parentNode->getTotalMatrix() * modelMatrix * animationMatrix;
    }
    else
        return modelMatrix * animationMatrix;
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
