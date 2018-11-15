#ifndef TERRAIN_H
#define TERRAIN_H

#include <QOpenGLFunctions_4_0_Core>
#include <QFile>
#include <QDataStream>
#include <QDebug>

class Terrain
{
public:
    Terrain(QFile *pgmFile);

    int width;
    int height;

private:
    GLuint heightMap;
};

#endif // TERRAIN_H
