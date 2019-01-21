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
    void updateTexture(QVector2D camPos, int radius);

    void loadFromFile(QFile *pgmFile);

    void drawTerrain(QOpenGLShaderProgram *prog, QVector3D camPos);

    void setSnowUniforms(QOpenGLShaderProgram *prog);

    float getMinimumSnowHeight() const;
    void setMinimumSnowHeight(float value);

    float getSnowGrowthRate() const;
    void setSnowGrowthRate(float value);

public slots:

    void replenishSnow();

protected:
    void generatePatches();
    void createSnowFallMaps();

    float minimumSnowHeight;
    float snowGrowthRate;

    int gridPosToPatch(int x, int y) {return x + y * terrainPatchesPerRow;}

    int terrainPatchesPerRow = (vertsPerRow ) * (heightMapSize + rowLength - 1) / rowLength; // Not floor

    std::vector<QImage> snowHeightMaps;
    std::vector<QImage> snowFallMaps;

    GLuint patchBuffer;
    GLuint snowHeightMapsTexture;

    GLint numLayers;
    GLint texSize;
};

#endif // SNOWTERRAIN_H
