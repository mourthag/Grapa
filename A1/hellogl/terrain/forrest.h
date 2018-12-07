#ifndef FORREST_H
#define FORREST_H


#include "tree.h"
#include "terrain.h"
#include "forrestdata.h"

class Forrest
{
public:
    Forrest();

    void setTree(Tree* tree);
    void draw(QOpenGLShaderProgram *treeProg);

    void generateTreeData(Terrain *terrain);
    void init();


    Tree* getTree();
    GLuint getTreeDataBuffer();

    int getNumTrees() {
        return parameters.numTrees;
    }

    int getMaxGeometryDistance() {
        return parameters.maxGeometryDistance;
    }
    int getMaxImpostorDistance() {
        return parameters.maxImpostorDistance;
    }
public slots:

    void setData(ForrestData data, Terrain *terrain);

private:
    bool isTreeValid(float x, float z, Terrain *terrain);

    std::vector<QVector4D> treeData;
    Tree* tree;

    ForrestData parameters;
    GLuint treeDataBuffer;

    bool isInited;
};

#endif // FORREST_H
