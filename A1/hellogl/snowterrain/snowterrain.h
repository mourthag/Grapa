#ifndef SNOWTERRAIN_H
#define SNOWTERRAIN_H

#include <terrain/terrain.h>
#include <util/openglfunctions.h>
#include <QImage>
#include <QPainter>
#include <QTimer>
#include <QObject>

class SnowTerrain : public Terrain
{

public:
    void initGL();
    void updateTexture();

    void drawTerrain(QOpenGLShaderProgram *prog, QVector3D camPos);

public slots:

    void replenishSnow();

private:
    std::vector<QImage> snowHeightMaps;

    GLuint snowHeightMapsTexture;

    GLint numLayers;
    GLint texSize;
};

#endif // SNOWTERRAIN_H
