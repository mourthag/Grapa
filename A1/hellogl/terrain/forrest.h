#ifndef FORREST_H
#define FORREST_H

#include <QOpenGLFunctions_4_3_Core>

#include "tree.h"
#include "terrain.h"

class Forrest : QOpenGLFunctions_4_3_Core
{
public:
    Forrest();
    Forrest(int numberTrees, float maxHeight, float maxSlope, float minScale, float maxScale, Terrain* terrain);

    void setTree(Tree* tree);
    void draw(QOpenGLShaderProgram *treeProg);

    Tree* getTree();
    GLuint getTreeDataBuffer();

    int treeAmount;
private:
    void generateTreeData(Terrain *terrain);
    bool isTreeValid(float x, float z, Terrain *terrain);

    std::vector<QVector4D> treeData;
    Tree* tree;

    float maximumHeight;
    float maximumSlope;
    float minimumScale;
    float maximumScale;

    GLuint treeDataBuffer;
};

#endif // FORREST_H
