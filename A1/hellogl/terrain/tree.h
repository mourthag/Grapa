#ifndef TREE_H
#define TREE_H

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


    QVector3D boundingBoxMin;
    QVector3D boundingBoxMax;
    float boundingSphereRadius;
    void calculateSphereRadius(QVector3D center);
    QVector4D boundingSphere;
};

#endif // TREE_H
