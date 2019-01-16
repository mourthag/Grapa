#include "snowterrain.h"

void SnowTerrain::initGL() {
    OpenGLFunctions *f = OpenGLFunctions::instance();

    f->glGetIntegerv(GL_MAX_ARRAY_TEXTURE_LAYERS, &numLayers);
    f->glGetIntegerv(GL_MAX_TEXTURE_SIZE, &texSize);

    texSize = 1024;
    numLayers = 20;
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

    f->glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGB8, texSize, texSize, numLayers,
                 0,
                 GL_RGB,
                 GL_UNSIGNED_BYTE,
                 0
                 );

    for(int i = 0; i < numLayers; i++) {
        snowHeightMaps.push_back(blackImage);

    }
    updateTexture();

}

void SnowTerrain::updateTexture() {
    OpenGLFunctions *f = OpenGLFunctions::instance();
    for(int i = 0; i < numLayers; i++) {
        f->glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, i, texSize, texSize, 1, GL_RGB, GL_UNSIGNED_BYTE, snowHeightMaps[i].bits());

    }
}

void SnowTerrain::drawTerrain(QOpenGLShaderProgram *prog, QVector3D camPos) {

    OpenGLFunctions *f = OpenGLFunctions::instance();

    prog->setUniformValue("snowMaps", 8);
    f->glActiveTexture(GL_TEXTURE8);
    f->glBindTexture(GL_TEXTURE_2D_ARRAY, snowHeightMapsTexture);
    prog->setUniformValue("patchNumber", 0);
    prog->setUniformValue("snowStartHeight", (GLfloat)50.0);
    prog->setUniformValue("snowGrowthRate", (GLfloat)0.1);

    Terrain::drawTerrain(prog, camPos);
}
