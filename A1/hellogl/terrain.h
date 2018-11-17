#ifndef TERRAIN_H
#define TERRAIN_H

#include <QOpenGLFunctions_4_0_Core>
#include <QOpenGLShaderProgram>

#include <QFile>
#include <QDataStream>
#include <QDebug>

class Terrain : QOpenGLFunctions_4_0_Core
{
public:
    Terrain(QFile *pgmFile);
    ~Terrain();

    int width;
    int height;

    void drawTerrain(QOpenGLShaderProgram *prog);

protected:
    void generatePatches();

private:
    const int vertsPerRow = 3;
    const int rowLength = 100;
    const int numpatches = 4 * (vertsPerRow - 1) * (vertsPerRow -1);

    GLuint heightMap;
    GLuint vao;
    GLuint vbo;
    GLuint ibo;

    void readInt(QDataStream *stream, int *result);
};

#endif // TERRAIN_H
