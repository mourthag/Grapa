#ifndef SNOWTERRAIN_H
#define SNOWTERRAIN_H

#include <terrain/terrain.h>
#include <util/openglfunctions.h>
#include <QImage>

class SnowTerrain : public Terrain
{
public:
    void initGL();
    void updateTexture();

    void drawTerrain(QOpenGLShaderProgram *prog, QVector3D camPos);

private:
    std::vector<QImage> snowHeightMaps;

    GLuint snowHeightMapsTexture;

    GLint numLayers;
    GLint texSize;
};

#endif // SNOWTERRAIN_H
