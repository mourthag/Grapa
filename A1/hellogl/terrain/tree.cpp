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
    }
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
