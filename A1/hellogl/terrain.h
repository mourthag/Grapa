#ifndef TERRAIN_H
#define TERRAIN_H

#include <QOpenGLFunctions_4_0_Core>
#include <QFile>
#include <QDataStream>
#include <QDebug>

class Terrain : QOpenGLFunctions_4_0_Core
{
public:
    Terrain(QFile *pgmFile);

    int width;
    int height;

private:
    GLuint heightMap;
    void readInt(QDataStream *stream, int *result);
};

#endif // TERRAIN_H
