#include "forrest.h"

Forrest::Forrest() {
    OpenGLFunctions *f = OpenGLFunctions::instance();
    isInited = false;

}

void Forrest::setData(ForrestData data, Terrain *terrain) {
    parameters = data;
    generateTreeData(terrain);

}

void Forrest::init() {
    if(isInited)
        return;

    OpenGLFunctions *f = OpenGLFunctions::instance();
    f->glGenBuffers(1, &treeDataBuffer);
    f->glBindBuffer(GL_ARRAY_BUFFER, treeDataBuffer);
    f->glBufferData(GL_ARRAY_BUFFER, sizeof(QVector4D) * treeData.size(), &treeData.at(0), GL_STATIC_DRAW);
    f->glBindBuffer(GL_ARRAY_BUFFER, 0);
    isInited = true;
}

Tree* Forrest::getTree() {
    return tree;
}

GLuint Forrest::getTreeDataBuffer() {
    return treeDataBuffer;
}

void Forrest::draw(QOpenGLShaderProgram *treeProg) {

    OpenGLFunctions *f = OpenGLFunctions::instance();

    QMatrix4x4 rotMat;
    rotMat.rotate(-90, 1, 0, 0);
    treeProg->setUniformValue("modelMat", rotMat);
    treeProg->setUniformValue("normalMat", rotMat.normalMatrix());

    for(int i=0; i < tree->meshes.size(); i++) {
        f->glBindVertexArray(tree->meshes[i]->vao);
        treeProg->setUniformValue("materialIndex", tree->meshes[i]->materialIndex);
        f->glDrawElementsInstanced(GL_TRIANGLES, tree->meshes[i]->num_verts, tree->meshes[i]->index_type, (void*)tree->meshes[i]->index_offset, treeData.size());
        f->glBindVertexArray(0);
    }
}

void Forrest::setTree(Tree *tree) {
    OpenGLFunctions *f = OpenGLFunctions::instance();

    this->tree = tree;
    std::vector<GLuint> vaos = tree->getVAOs();

    for(int i = 0; i < vaos.size(); i++) {
        f->glBindVertexArray(vaos[i]);

        f->glBindBuffer(GL_ARRAY_BUFFER, treeDataBuffer);
        f->glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 0, 0);
        f->glEnableVertexAttribArray(4);
        f->glVertexAttribDivisor(4, 1);

        f->glBindVertexArray(0);
        f->glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
}

void Forrest::generateTreeData(Terrain* terrain) {

    if(terrain == NULL)
        return;

    treeData.clear();

    QVector2D terrainSize = terrain->getSize();

    for(int i = 0; i < parameters.numTrees; i++) {
        float xCoord = 0;
        float zCoord = 0;
        do {
            xCoord = rand() % (int)terrainSize.x();
            zCoord = rand() % (int)terrainSize.y();
        }while(isTreeValid(xCoord, zCoord, terrain));

        float scale = parameters.minScale + fmod(rand() , (parameters.maxScale - parameters.minScale));
        float angle = rand() % 360;
        QVector4D data(xCoord, zCoord, scale, angle);
        treeData.push_back(data);
    }

    init();
    OpenGLFunctions *f = OpenGLFunctions::instance();
    f->glBindBuffer(GL_ARRAY_BUFFER, treeDataBuffer);
    f->glBufferData(GL_ARRAY_BUFFER, sizeof(QVector4D) * treeData.size(), &treeData.at(0), GL_STATIC_DRAW);
}

bool Forrest::isTreeValid(float x, float z, Terrain *terrain) {
    QVector2D gridPos(x,  z);
    float height = terrain->getHeight(gridPos);
    float slope = terrain->getSlope(gridPos);
    return (height <= parameters.maxHeight) && (slope >= parameters.maxSlope);
}
