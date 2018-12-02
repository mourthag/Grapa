#ifndef FORREST_H
#define FORREST_H

#include <QOpenGLFunctions_4_0_Core>

#include "tree.h"
#include "terrain.h"

class Forrest : QOpenGLFunctions_4_0_Core
{
public:
    Forrest(int numberTrees, float maxHeight, float maxSlope, float minScale, float maxScale, Terrain* terrain);

    void setTree(Tree* tree);
    void draw(QOpenGLShaderProgram *treeProg);
private:
    void generateTreeData(Terrain *terrain);
    bool isTreeValid(float x, float z, Terrain *terrain);

    std::vector<QVector4D> treeData;
    Tree* tree;

    int treeAmount;
    float maximumHeight;
    float maximumSlope;
    float minimumScale;
    float maximumScale;

    GLuint treeDataBuffer;
};

#endif // FORREST_H
