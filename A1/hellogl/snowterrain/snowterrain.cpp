#include "snowterrain.h"

void SnowTerrain::initGL() {
    OpenGLFunctions *f = OpenGLFunctions::instance();

    f->glGetIntegerv(GL_MAX_ARRAY_TEXTURE_LAYERS, &numLayers);
    f->glGetIntegerv(GL_MAX_TEXTURE_SIZE, &texSize);

    texSize = 512;
    numLayers = 50*50;
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

    QTimer *snowTimer = new QTimer();
    QObject::connect(snowTimer, &QTimer::timeout, [ = ]() {
        replenishSnow();
    } );
    snowTimer->start(10000);



}

void SnowTerrain::updateTexture() {
    OpenGLFunctions *f = OpenGLFunctions::instance();
    f->glBindTexture(GL_TEXTURE_2D_ARRAY, snowHeightMapsTexture);
    for(int i = 0; i < numLayers; i++) {
        f->glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, i, texSize, texSize, 1, GL_RGB, GL_UNSIGNED_BYTE, snowHeightMaps[i].bits());

    }
}

void SnowTerrain::replenishSnow() {
    Qt::BrushStyle patterns[3] {Qt::Dense2Pattern, Qt::Dense6Pattern, Qt::Dense7Pattern};
    for(int i = 0; i < snowHeightMaps.size(); i++) {
        int random = rand() % 3;

        Qt::BrushStyle pattern = patterns[random];

        QPainter painter;
        painter.begin(&snowHeightMaps[i]);
        painter.setBrush(QBrush(QColor(255,255,255,100), pattern));
        painter.drawRect(0, 0, texSize, texSize);
    }
    snowHeightMaps[0].save("test.png");
    updateTexture();
}

void SnowTerrain::drawTerrain(QOpenGLShaderProgram *prog, QVector3D camPos) {

    OpenGLFunctions *f = OpenGLFunctions::instance();

    prog->setUniformValue("snowMaps", 8);
    f->glActiveTexture(GL_TEXTURE8);
    f->glBindTexture(GL_TEXTURE_2D_ARRAY, snowHeightMapsTexture);
    prog->setUniformValue("patchNumber", 0);
    prog->setUniformValue("snowStartHeight", (GLfloat)50.0);
    prog->setUniformValue("snowGrowthRate", (GLfloat)1.0);

    Terrain::drawTerrain(prog, camPos);
}
