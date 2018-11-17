#include "terrain.h"

Terrain::~Terrain() {
    glDeleteVertexArrays(1, &vao);
    glDeleteTextures(1, &heightMap);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ibo);


}

void Terrain::readInt(QDataStream *stream, int *result)
{
    char widthText[10];

    for( int i = 0 ; i < 10; i++) {
        stream->readRawData(&widthText[i], 1);
        if(widthText[i] == '\0' || widthText[i] == '\n')
            break;
    }

    sscanf(widthText, "%d", result);
}

Terrain::Terrain(QFile *pgmFile)
{
    initializeOpenGLFunctions();

    pgmFile->open(QIODevice::ReadOnly);
    QDataStream stream(pgmFile);

    char format[2];

    stream.readRawData(format, 2);

    //skip \n
    stream.skipRawData(1);

    int width;
    readInt(&stream, &width);

    int height;
    readInt(&stream, &height);

    int maxVal;
    readInt(&stream, &maxVal);

    if( maxVal  != 65535) {
        qDebug() << "Invalid Values";
        return;
    }

    std::vector<char> data(2 * width * height);
    stream.readRawData(&data.at(0), 2 * width * height);

    std::vector<unsigned short> heights;

    for(int i = 0; i < width * height * 2; i = i + 2) {
        unsigned short value;
        value = (unsigned short)data[i];
        value <<=  CHAR_BIT;
        value += (unsigned short)data[i+1];
        heights.push_back(value);
    }

    generatePatches();

    glGenTextures(1, &heightMap);
    glBindTexture(GL_TEXTURE_2D, heightMap);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R16UI, width, height, 0, GL_RED_INTEGER, GL_UNSIGNED_SHORT, &heights.at(0));
    glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D,0);

}

void Terrain::drawTerrain(QOpenGLShaderProgram *prog) {


    prog->setUniformValue("heightMap", 4);
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, heightMap);
    prog->setUniformValue("modelMat", QMatrix4x4());

    glBindVertexArray(vao);

    glPatchParameteri(GL_PATCH_VERTICES, 4);
    glDrawElements(GL_PATCHES, numpatches, GL_UNSIGNED_INT, 0 );
    glBindVertexArray(0);

}

void Terrain::generatePatches() {
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    std::vector<GLfloat> vertices;
    std::vector<GLuint> indices;

    float distanceBetweenVerts = (float)rowLength / (float)(vertsPerRow-1);
    float offset = (vertsPerRow - 1.0) / 2.0;

    for(int column=0; column < vertsPerRow; column++)  {
        for(int row  = 0; row < vertsPerRow; row++) {

            QVector2D pos = QVector2D((offset - (float)row) * distanceBetweenVerts, (offset - (float)column) * distanceBetweenVerts);
            vertices.push_back(pos.x());
            vertices.push_back(pos.y());

            if(row != vertsPerRow - 1 && column != vertsPerRow -1) {
                int index = column * vertsPerRow + row;
                indices.push_back(index);
                indices.push_back(index+vertsPerRow);
                indices.push_back(index+1+vertsPerRow);
                indices.push_back(index+1);
            }
        }
    }

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)* vertices.size(), &vertices.at(0), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indices.size(), &indices.at(0), GL_STATIC_DRAW);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
