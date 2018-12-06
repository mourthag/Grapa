#ifndef TREE_H
#define TREE_H

#include <QOpenGLFunctions_4_3_Core>

#include "tiny_gltf.h"

#include <scenegraph/openglmodel.h>

class Tree
{
public:
    Tree();

    void loadTree(tinygltf::Model *gltf_model, int meshIndex);
    std::vector<GLuint> getVAOs();

    std::vector<GLuint> getVertexCounts(GLuint minSize);

    std::vector<OpenGLModel*> meshes;

};

#endif // TREE_H
