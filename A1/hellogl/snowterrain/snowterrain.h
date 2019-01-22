#ifndef SNOWTERRAIN_H
#define SNOWTERRAIN_H

#include <terrain/terrain.h>
#include <util/openglfunctions.h>
#include <QImage>
#include <QGraphicsBlurEffect>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QPainter>
#include <QTimer>
#include <QObject>

class SnowTerrain : public Terrain
{

public:
    void initGL();
    void updateTexture(QVector2D camPos, int radius);

    void loadFromFile(QFile *pgmFile);

    void setSnowUniforms(QOpenGLShaderProgram *prog);
    void drawTerrain(QOpenGLShaderProgram *prog, QVector3D camPos);

    void leaveFootprint(QVector3D pos);

    float getMinimumSnowHeight() const;
    void setMinimumSnowHeight(float value);

    float getSnowGrowthRate() const;
    void setSnowGrowthRate(float value);

public slots:

    void replenishSnow(QVector2D camPos, int radius);

protected:
    QImage applyBlurToImage(QImage src);

    void generatePatches();
    void createSnowFallMaps();

    float minimumSnowHeight;
    float snowGrowthRate;

    int gridPosToPatch(int x, int y) {return x + y * terrainPatchesPerRow;}

    int terrainPatchesPerRow; // Not floor

    std::vector<QImage> snowHeightMaps;
    std::vector<QImage> snowFallMaps;

    QImage footprintImg;

    GLuint patchBuffer;
    GLuint snowHeightMapsTexture;

    GLint numLayers;
    GLint texSize;
};

#endif // SNOWTERRAIN_H
