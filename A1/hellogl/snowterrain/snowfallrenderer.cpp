#include "snowfallrenderer.h"

static const GLfloat quad_UV[] = {
    0.0f, 0.0f,
    1.0f, 0.0f,
    0.0f, 1.0f,
    1.0f, 1.0f
};

static const GLuint quad_Ind[] = {
    0, 1, 2, 3
};

void SnowFallRenderer::initGL() {

    time.start();

    radius = 10;

    float randX = (float)(rand() % 100) / 100.0;
    float randY = (float)(rand() % 100) / 100.0;

    windDirection = QVector3D(randX - 0.5, 0,randY - 0.5);
    fallingSpeed = 0.35f;

    OpenGLFunctions *f = OpenGLFunctions::instance();

    f->glGenVertexArrays(1, &particleVAO);
    f->glBindVertexArray(particleVAO);

    f->glGenBuffers(1, &particleVBO);
    f->glBindBuffer(GL_ARRAY_BUFFER, particleVBO);
    f->glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(GLfloat), &quad_UV, GL_STATIC_DRAW);
    f->glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
    f->glEnableVertexAttribArray(0);

    f->glGenBuffers(1, &particleIBO);
    f->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, particleIBO);
    f->glBufferData(GL_ELEMENT_ARRAY_BUFFER, 4 * sizeof(GLuint), &quad_Ind, GL_STATIC_DRAW);

    f->glGenBuffers(1, &particleDataBuffer);
    f->glBindBuffer(GL_ARRAY_BUFFER, particleDataBuffer);
    f->glBufferData(GL_ARRAY_BUFFER, numParticles * 4 * sizeof(GL_FLOAT), NULL, GL_STATIC_DRAW);
    f->glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, 0);
    f->glEnableVertexAttribArray(1);
    f->glVertexAttribDivisor(1, 1);

    f->glBindVertexArray(0);

    QImage snowflakeImg(":/img/snowflake.png");

    f->glGenTextures(1, &snowflakeTex);
    f->glBindTexture(GL_TEXTURE_2D, snowflakeTex);

    f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    f->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, snowflakeImg.width(), snowflakeImg.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, snowflakeImg.bits());
    f->glBindTexture(GL_TEXTURE_2D, 0);

}

void SnowFallRenderer::setHeight(float height, float minSnowHeight, float snowGrowth) {
    if(height < minSnowHeight) {
        numActiveParticles = 0;
        return;
    }
    else {
        numActiveParticles = numMinParticles + snowGrowth * (height - minSnowHeight);
        numActiveParticles = std::min(numActiveParticles, numParticles);
    }
}

int SnowFallRenderer::getNumParticles() const
{
    return numParticles;
}

void SnowFallRenderer::setNumParticles(int value)
{
    numParticles = value;
    createRandomParticles();
    updateBuffer();
}

int SnowFallRenderer::getNumMinParticles() const
{
    return numMinParticles;
}

void SnowFallRenderer::setNumMinParticles(int value)
{
    numMinParticles = value;
}

void SnowFallRenderer::updateBuffer() {

    OpenGLFunctions *f = OpenGLFunctions::instance();

    f->glBindVertexArray(particleVAO);
    f->glBindBuffer(GL_ARRAY_BUFFER, particleDataBuffer);
    f->glBufferData(GL_ARRAY_BUFFER, particleData.size() * sizeof(GLfloat), &particleData.at(0), GL_STATIC_DRAW);
    f->glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, 0);
    f->glVertexAttribDivisor(1, 1);
    f->glBindVertexArray(0);
    f->glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void SnowFallRenderer::createRandomParticles() {

    particleData.clear();

    float precision = 100;

    for(int i=0; i < numParticles; i++) {
        float x = (float)(rand() % (2 * radius * (int)precision)) / precision;
        x -= radius;
        float y = (float)(rand() % (2 * radius * (int)precision)) / precision;
        y -= radius;
        float z = (float)(rand() % (2 * radius * (int)precision)) / precision;
        z -= radius;
        float rotation = rand() % 360;

        particleData.push_back(x);
        particleData.push_back(y);
        particleData.push_back(z);
        particleData.push_back(rotation);
    }
}

void SnowFallRenderer::drawParticles(QOpenGLShaderProgram *prog) {

    OpenGLFunctions *f = OpenGLFunctions::instance();

    prog->setUniformValue("radius", (GLfloat)radius);
    prog->setUniformValue("windDir", windDirection);
    prog->setUniformValue("fallingSpeed", fallingSpeed);
    prog->setUniformValue("timeElapsed", (GLfloat)((float)time.elapsed() / 1000.0));

    prog->setUniformValue("snowflakeTexture", 9);
    f->glActiveTexture(GL_TEXTURE9);
    f->glBindTexture(GL_TEXTURE_2D, snowflakeTex);

    f->glBindVertexArray(particleVAO);
    f->glDrawElementsInstanced(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_INT, 0, std::min(numActiveParticles, numParticles));
    f->glBindVertexArray(0);

}
