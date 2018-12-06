#ifndef TERRAIN_H
#define TERRAIN_H

#include <QOpenGLFunctions_4_3_Core>
#include <QOpenGLShaderProgram>

#include <QFile>
#include <QDataStream>
#include <QDebug>
#include <QImage>

class Terrain : QOpenGLFunctions_4_3_Core
{
public:
    Terrain();
    Terrain(QFile *pgmFile);
    ~Terrain();

    int width;
    int height;

    void loadFromFile(QFile *pgmFile);
    void drawTerrain(QOpenGLShaderProgram *prog, QVector3D camPos);
    void setHeightMapUniform(QOpenGLShaderProgram *prog);
    float getHeight(QVector2D gridPos);
    float getSlope(QVector2D gridPos);
    QVector3D getNormal(QVector2D gridPos);
    QVector2D getSize();

protected:
    void generatePatches();

private:
    const int vertsPerRow = 50;
    const int rowLength = 1000;
    const int numpatches = 4 * (vertsPerRow - 1) * (vertsPerRow -1);
    const float distanceBetweenVerts = (float)rowLength / (float)(vertsPerRow-1);

    GLuint heightMap;
    GLuint vao;
    GLuint vbo;
    GLuint ibo;
    GLuint materialArrayTexture;

    int heightMapSize;
    std::vector<unsigned short> heights;

    void readInt(QDataStream *stream, int *result);
    void createHeightMap(int width, QDataStream *stream, int height);
};

#endif // TERRAIN_H
