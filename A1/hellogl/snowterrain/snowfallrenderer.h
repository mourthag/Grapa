#ifndef SNOWFALLRENDERER_H
#define SNOWFALLRENDERER_H

#include <QOpenGLShaderProgram>
#include <QImage>
#include <QTime>
#include "util/openglfunctions.h"

class SnowFallRenderer
{
public:
    void initGL();

    void createRandomParticles();
    void updateBuffer();
    void drawParticles(QOpenGLShaderProgram *prog);
    void setHeight(float height, float minSnowHeight, float snowGrowth);

protected:

    int numParticles = 5000;
    int numMinParticles = 100;
    int numActiveParticles;
    int radius;

    QTime time;

    QVector3D windDirection;
    float fallingSpeed;

    std::vector<GLfloat> particleData;

    GLuint snowflakeTex;

    GLuint particleVAO;
    GLuint particleVBO;
    GLuint particleIBO;
    GLuint particleDataBuffer;
};

#endif // SNOWFALLRENDERER_H
