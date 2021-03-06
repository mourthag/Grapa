#include "tree.h"

Tree::Tree()
{

}

void Tree::loadTree(tinygltf::Model *gltf_model, int meshIndex) {
    tinygltf::Mesh *mesh = &gltf_model->meshes[meshIndex];
    for(int i = 0; i < mesh->primitives.size(); i++) {

        OpenGLModel *oglModel = (OpenGLModel*)malloc(sizeof(OpenGLModel));
        oglModel = new OpenGLModel();
        oglModel->loadGLTF(gltf_model, meshIndex, i, false);
        meshes.push_back(oglModel);
        boundingBoxMin.setX(std::min(boundingBoxMin.x(), oglModel->getBoundingBoxMinimum().x()));
        boundingBoxMin.setY(std::min(boundingBoxMin.y(), oglModel->getBoundingBoxMinimum().y()));
        boundingBoxMin.setZ(std::min(boundingBoxMin.z(), oglModel->getBoundingBoxMinimum().z()));
        boundingBoxMax.setX(std::max(boundingBoxMax.x(), oglModel->getBoundingBoxMaximum().x()));
        boundingBoxMax.setY(std::max(boundingBoxMax.y(), oglModel->getBoundingBoxMaximum().y()));
        boundingBoxMax.setZ(std::max(boundingBoxMax.z(), oglModel->getBoundingBoxMaximum().z()));
    }
    QVector3D center = 0.5 * boundingBoxMin + 0.5 * boundingBoxMax;
    calculateSphereRadius(center);
    boundingSphere = QVector4D(center, boundingSphereRadius);
}

std::vector<GLuint> Tree::getVertexCounts(GLuint minSize) {
    std::vector<GLuint> numVertices(minSize);
    for(int i=0; i <meshes.size(); i++) {
        numVertices[i] = meshes[i]->num_verts;
    }
    return numVertices;
}

std::vector<GLuint> Tree::getVAOs() {
    std::vector<GLuint> vaos;
    for(int i=0; i <meshes.size(); i++) {
        vaos.push_back(meshes[i]->vao);
    }
    return vaos;
}

void Tree::calculateSphereRadius(QVector3D center) {
    float maxDistance = 0;
    for(int i=0; i< meshes.size(); ++i) {
        std::vector<QVector3D> *data = &meshes[i]->vertices;
        for(int j=0; j < data->size(); ++j) {
            float distance = data->at(j).distanceToPoint(center);
            maxDistance = std::max(distance, maxDistance);
        }
    }
    boundingSphereRadius =  maxDistance;
}
