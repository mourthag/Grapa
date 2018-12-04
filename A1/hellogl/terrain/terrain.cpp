#include "terrain.h"

Terrain::Terrain() {
}

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

void Terrain::createHeightMap(int width, QDataStream *stream, int height)
{
    std::vector<char> data(2 * width * height);
    stream->readRawData(&data.at(0), 2 * width * height);

    for(int i = 0; i < width * height * 2; i = i + 2) {
        unsigned short value;
        value = (unsigned short)data[i];
        value <<=  CHAR_BIT;
        value += (unsigned short)data[i+1];
        heights.push_back(value);
    }


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

Terrain::Terrain(QFile *pgmFile)
{
    initializeOpenGLFunctions();
    loadFromFile(pgmFile);

}

void Terrain::loadFromFile(QFile *pgmFile) {

    initializeOpenGLFunctions();
    pgmFile->open(QIODevice::ReadOnly);
    QDataStream stream(pgmFile);

    char format[2];

    stream.readRawData(format, 2);

    //skip \n
    stream.skipRawData(1);

    int width;
    readInt(&stream, &width);

    heightMapSize = width;

    int height;
    readInt(&stream, &height);

    int maxVal;
    readInt(&stream, &maxVal);


    if( maxVal  != 65535) {
        qDebug() << "Invalid Values";
        return;
    }

    createHeightMap(width, &stream, height);

    generatePatches();

    QString dir = pgmFile->fileName().section("/",0,-2);
    QImage rockImage(dir + "/rock.jpg");
    QImage gravelImage(dir + "/gravel.jpg");
    QImage sandImage(dir + "/sand.jpg");
    QImage stoneImage(dir + "/stone.jpg");

    glGenTextures(1, &materialArrayTexture);
    glBindTexture(GL_TEXTURE_2D_ARRAY, materialArrayTexture);

    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA8, rockImage.width(), rockImage.height(),
                 4,
                 0,
                 GL_RGBA,
                 GL_UNSIGNED_BYTE,
                 0);
    glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, 0, rockImage.width(), rockImage.height(), 1, GL_RGBA, GL_UNSIGNED_BYTE, rockImage.bits());
    glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, 1, gravelImage.width(), gravelImage.height(), 1, GL_RGBA, GL_UNSIGNED_BYTE, gravelImage.bits());
    glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, 2, sandImage.width(), sandImage.height(), 1, GL_RGBA, GL_UNSIGNED_BYTE, sandImage.bits());
    glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, 3, stoneImage.width(), stoneImage.height(), 1, GL_RGBA, GL_UNSIGNED_BYTE, stoneImage.bits());

    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
}

float Terrain::getHeight(QVector2D gridPos) {

    int x = std::max(std::min((int)(gridPos.x() + 0.5), heightMapSize), 0);
    int y = std::max(std::min((int)(gridPos.y() + 0.5), heightMapSize), 0);

    int index = x * heightMapSize + y;
    float height = (float)heights.at(index);
    return height;

}

QVector3D Terrain::getNormal(QVector2D gridPos) {
    QVector2D neighbourXGrid = gridPos + QVector2D(1,0);
    QVector2D neighbourZGrid = gridPos + QVector2D(0,1);

    float heightPos = getHeight(gridPos);
    float heightX = getHeight(neighbourXGrid);
    float heightZ = getHeight(neighbourZGrid);

    QVector3D pos(gridPos.x(), heightPos, gridPos.y());
    QVector3D neighbourX(neighbourXGrid.x(), heightX, neighbourXGrid.y());
    QVector3D neighbourZ(neighbourZGrid.x(), heightZ, neighbourZGrid.y());

    QVector3D normal = QVector3D::crossProduct(
                                     neighbourX-pos,
                                     neighbourZ-pos);
    normal.normalize();
    return normal;
}


float Terrain::getSlope(QVector2D gridPos) {
    return getNormal(gridPos).y();
}

void Terrain::setHeightMapUniform(QOpenGLShaderProgram *prog) {

    prog->setUniformValue("heightMap", 4);
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, heightMap);

}

void Terrain::drawTerrain(QOpenGLShaderProgram *prog, QVector3D camPos) {

    setHeightMapUniform(prog);

    prog->setUniformValue("materialTextures", 5);
    glActiveTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_2D_ARRAY, materialArrayTexture);

    int xFactor = camPos.x()/distanceBetweenVerts;
    int zFactor = camPos.z() / distanceBetweenVerts;

    camPos.setX(xFactor * distanceBetweenVerts);
    camPos.setZ(zFactor * distanceBetweenVerts);

    QMatrix4x4 camTranslationMatrix;
    camTranslationMatrix.translate(camPos);
    prog->setUniformValue("modelMat", camTranslationMatrix);
    prog->setUniformValue("patchSize", distanceBetweenVerts);
    prog->setUniformValue("heightMapSize", (GLfloat)heightMapSize);

    prog->setUniformValue("rockShininess", (GLfloat)12.0);
    prog->setUniformValue("rockSpecular", QVector3D(0.5, 0.5, 0.5));
    prog->setUniformValue("rockSlope", (GLfloat)0.5);
    prog->setUniformValue("rockMargin", (GLfloat)0.1);

    prog->setUniformValue("rockShininess", (GLfloat)120.0);
    prog->setUniformValue("rockSpecular", QVector3D(0.5, 0.5, 0.5));
    prog->setUniformValue("stoneSlope", (GLfloat)0.8);
    prog->setUniformValue("stoneMargin", (GLfloat)0.18);

    prog->setUniformValue("sandShininess", (GLfloat)20.0);
    prog->setUniformValue("sandSpecular", QVector3D(0.1, 0.1, 0.1));
    prog->setUniformValue("sandHeight", (GLfloat)17.0);
    prog->setUniformValue("sandMargin", (GLfloat)1.5);

    prog->setUniformValue("gravelShininess", (GLfloat)10.0);
    prog->setUniformValue("gravelSpecular", QVector3D(0.1, 0.1, 0.1));
    prog->setUniformValue("gravelHeight", (GLfloat)20.5);
    prog->setUniformValue("gravelMargin", (GLfloat)1.5);

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

QVector2D Terrain::getSize() {
    return QVector2D(heightMapSize, heightMapSize);
}
