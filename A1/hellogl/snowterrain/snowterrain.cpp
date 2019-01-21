#include "snowterrain.h"

void SnowTerrain::loadFromFile(QFile *pgmFile) {

    OpenGLFunctions *f = OpenGLFunctions::instance();

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

    createSnowFallMaps();

    QString dir = pgmFile->fileName().section("/",0,-2);
    QImage rockImage(dir + "/rock.jpg");
    QImage gravelImage(dir + "/gravel.jpg");
    QImage sandImage(dir + "/sand.jpg");
    QImage stoneImage(dir + "/stone.jpg");
    QImage snowImage(dir + "/snow.jpg");

    f->glGenTextures(1, &materialArrayTexture);
    f->glBindTexture(GL_TEXTURE_2D_ARRAY, materialArrayTexture);

    f->glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    f->glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    f->glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
    f->glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);

    f->glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA8, rockImage.width(), rockImage.height(),
                 5,
                 0,
                 GL_RGBA,
                 GL_UNSIGNED_BYTE,
                 0);
    f->glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, 0, rockImage.width(), rockImage.height(), 1, GL_RGBA, GL_UNSIGNED_BYTE, rockImage.bits());
    f->glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, 1, gravelImage.width(), gravelImage.height(), 1, GL_RGBA, GL_UNSIGNED_BYTE, gravelImage.bits());
    f->glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, 2, sandImage.width(), sandImage.height(), 1, GL_RGBA, GL_UNSIGNED_BYTE, sandImage.bits());
    f->glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, 3, stoneImage.width(), stoneImage.height(), 1, GL_RGBA, GL_UNSIGNED_BYTE, stoneImage.bits());
    f->glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, 4, snowImage.width(), snowImage.height(), 1, GL_RGBA, GL_UNSIGNED_BYTE, snowImage.bits());

    f->glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
}

void SnowTerrain::generatePatches() {
    OpenGLFunctions *f = OpenGLFunctions::instance();

    f->glBindVertexArray(vao);

    std::vector<GLfloat> vertices;
    std::vector<GLuint> indices;
    std::vector<GLfloat> patchOffsets;

    float offset = (vertsPerRow - 1.0) / 2.0;

    for(int column=0; column < vertsPerRow; column++)  {
        for(int row  = 0; row < vertsPerRow; row++) {

            QVector2D pos = QVector2D((offset - (float)row) * distanceBetweenVerts, (offset - (float)column) * distanceBetweenVerts);
            vertices.push_back(pos.x());
            vertices.push_back(pos.y());

            int patchOffsetX = pos.x() / distanceBetweenVerts;
            int patchOffsetZ = pos.y() / distanceBetweenVerts;
            patchOffsets.push_back(patchOffsetX);
            patchOffsets.push_back(patchOffsetZ);

            if(row != vertsPerRow - 1 && column != vertsPerRow -1) {
                int index = column * vertsPerRow + row;
                indices.push_back(index);
                indices.push_back(index+vertsPerRow);
                indices.push_back(index+1+vertsPerRow);
                indices.push_back(index+1);
            }
        }
    }

    f->glBindBuffer(GL_ARRAY_BUFFER, vbo);
    f->glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)* vertices.size(), &vertices.at(0), GL_STATIC_DRAW);
    f->glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
    f->glEnableVertexAttribArray(0);

    f->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    f->glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indices.size(), &indices.at(0), GL_STATIC_DRAW);

    f->glBindBuffer(GL_ARRAY_BUFFER, patchBuffer);
    f->glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * patchOffsets.size(), &patchOffsets.at(0), GL_STATIC_DRAW);
    f->glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
    f->glEnableVertexAttribArray(1);

    f->glBindVertexArray(0);
    f->glBindBuffer(GL_ARRAY_BUFFER, 0);
    f->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void SnowTerrain::initGL() {

    minimumSnowHeight = 50.0;
    snowGrowthRate = 0.1;

    OpenGLFunctions *f = OpenGLFunctions::instance();


    f->glGenVertexArrays(1, &vao);
    f->glGenBuffers(1, &vbo);
    f->glGenBuffers(1, &ibo);
    f->glGenBuffers(1, &patchBuffer);

    f->glGetIntegerv(GL_MAX_ARRAY_TEXTURE_LAYERS, &numLayers);
    f->glGetIntegerv(GL_MAX_TEXTURE_SIZE, &texSize);

    texSize = 256;
    numLayers = terrainPatchesPerRow * terrainPatchesPerRow;
    qDebug() << numLayers << texSize;

    snowHeightMaps.clear();

    f->glGenTextures(1, &snowHeightMapsTexture);
    f->glBindTexture(GL_TEXTURE_2D_ARRAY, snowHeightMapsTexture);

    f->glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    f->glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    f->glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
    f->glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);

    QImage blackImage(texSize, texSize, QImage::Format_RGB888);
    blackImage.fill(Qt::black);
    QImage whiteImage(texSize, texSize, QImage::Format_RGBA8888);
    whiteImage.fill(Qt::white);

    f->glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGB8, texSize, texSize, numLayers,
                 0,
                 GL_RGB,
                 GL_UNSIGNED_BYTE,
                 0
                 );

    for(int i = 0; i < numLayers; i++) {
        snowHeightMaps.push_back(blackImage);
        snowFallMaps.push_back(whiteImage);

    }
}

void SnowTerrain::createSnowFallMaps() {

    float pixelOffestInWorld = (float)distanceBetweenVerts / (float)texSize;

    for(int y = 0; y < terrainPatchesPerRow  ; y++) {
        for(int x = 0; x < terrainPatchesPerRow ; x++) {

            QVector2D baseWorldPos(x * distanceBetweenVerts, y *distanceBetweenVerts);
            int patch = gridPosToPatch(x, y);

            for(int v = 0; v < texSize; v++) {
                for(int u = 0; u < texSize; u++) {
                    QVector2D uv(u,v);
                    float slope = std::max(getNormal(baseWorldPos + pixelOffestInWorld * uv).y(), 0.f);

                    snowFallMaps[patch].setPixelColor(u, v, QColor(255, 255, 255, slope * 255));

                }
            }
        }

    }
}

float SnowTerrain::getSnowGrowthRate() const
{
    return snowGrowthRate;
}

void SnowTerrain::setSnowGrowthRate(float value)
{
    snowGrowthRate = value;
}

float SnowTerrain::getMinimumSnowHeight() const
{
    return minimumSnowHeight;
}

void SnowTerrain::setMinimumSnowHeight(float value)
{
    minimumSnowHeight = value;
}

void SnowTerrain::updateTexture(QVector2D camPos, int radius) {

    QVector2D baseGridPos = camPos / distanceBetweenVerts;

    OpenGLFunctions *f = OpenGLFunctions::instance();
    f->glBindTexture(GL_TEXTURE_2D_ARRAY, snowHeightMapsTexture);

    for(int i = 0; i < terrainPatchesPerRow; i++) {
        for(int j = 0; j < terrainPatchesPerRow; j++) {
            if(i < 0 || j < 0 || j >= terrainPatchesPerRow || i >= terrainPatchesPerRow) {
                continue;
            }
            int patch = gridPosToPatch(j, i);

            qDebug() << patch << "loaded into memory";
            f->glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0,
                               0, 0, patch,
                               texSize, texSize, 1,
                               GL_RGB,
                               GL_UNSIGNED_BYTE,
                               snowHeightMaps[patch].bits());
        }

    }
    f->glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
}

void SnowTerrain::replenishSnow() {
    for(int i = 0; i < snowHeightMaps.size(); i++) {


        QPainter painter;
        painter.begin(&snowHeightMaps[i]);
        painter.drawImage(QPoint(0,0), snowFallMaps[i]);
    }
}

void SnowTerrain::setSnowUniforms(QOpenGLShaderProgram *prog)
{
    OpenGLFunctions *f = OpenGLFunctions::instance();

    prog->setUniformValue("snowMaps", 8);
    f->glActiveTexture(GL_TEXTURE8);
    f->glBindTexture(GL_TEXTURE_2D_ARRAY, snowHeightMapsTexture);
    prog->setUniformValue("snowSpecular", QVector3D(0.1,0.1,0.1));
    prog->setUniformValue("snowShininess", (GLfloat)5.0);
    prog->setUniformValue("snowStartHeight", (GLfloat)minimumSnowHeight);
    prog->setUniformValue("snowGrowthRate", (GLfloat)snowGrowthRate);
}

void SnowTerrain::drawTerrain(QOpenGLShaderProgram *prog, QVector3D camPos) {


    setSnowUniforms(prog);

    prog->setUniformValue("patchesPerRow", terrainPatchesPerRow);

    Terrain::drawTerrain(prog, camPos);
}
