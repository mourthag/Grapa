#include "forrest.h"

Forrest::Forrest() {

}

Forrest::Forrest(int numberTrees, float maxHeight, float maxSlope, float minScale, float maxScale, Terrain *terrain)
{
    treeAmount = numberTrees;
    maximumHeight = maxHeight;
    maximumSlope = maxSlope;
    minimumScale = minScale;
    maximumScale = maxScale;

    generateTreeData(terrain);

    initializeOpenGLFunctions();
    glGenBuffers(1, &treeDataBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, treeDataBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(QVector4D) * treeData.size(), &treeData.at(0), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Forrest::draw(QOpenGLShaderProgram *treeProg) {

    std::vector<GLuint> vaos = tree->getVAOs();
    for(int i=0; i < vaos.size(); i++) {
        glBindVertexArray(vaos[i]);
        glDrawElementsInstanced(GL_TRIANGLES, 3*tree->meshes[i]->num_tris, tree->meshes[i]->index_type, (void*)tree->meshes[i]->index_offset, treeAmount);
        glBindVertexArray(0);
    }
}

void Forrest::setTree(Tree *tree) {
    this->tree = tree;
    std::vector<GLuint> vaos = tree->getVAOs();

    for(int i = 0; i < vaos.size(); i++) {
        glBindVertexArray(vaos[i]);

        glBindBuffer(GL_ARRAY_BUFFER, treeDataBuffer);
        glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(4);
        glVertexAttribDivisor(4, 1);

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
}

void Forrest::generateTreeData(Terrain* terrain) {

    treeData.clear();

    QVector2D terrainSize = terrain->getSize();

    for(int i = 0; i < treeAmount; i++) {
        float xCoord = 0;
        float zCoord = 0;
        do {
            xCoord = rand() * terrainSize.x();
            zCoord = rand() * terrainSize.y();
        }while(isTreeValid(xCoord, zCoord, terrain));

        float scale = minimumScale + rand() * (maximumScale - minimumScale);
        float angle = 360.0 * rand();
        QVector4D data(xCoord, zCoord, scale, angle);
        treeData.push_back(data);
    }
}

bool Forrest::isTreeValid(float x, float z, Terrain *terrain) {
    QVector2D gridPos(x,  z);
    float height = terrain->getHeight(gridPos);
    float slope = terrain->getSlope(gridPos);
    return (height <= maximumHeight) && (slope <= maximumSlope);
}
