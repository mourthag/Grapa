#ifndef SCENERENDERER_H
#define SCENERENDERER_H

#include <QOpenGLFunctions_4_0_Core>
#include <QDebug>
#include <QOpenGLShaderProgram>

#include "scene.h"
#include "performancechart.h"


class SceneRenderer : QOpenGLFunctions_4_0_Core
{
public:
    SceneRenderer();
    void initGL();

    enum RenderMode {

        Deferred=0,
        Normal=1,
        UV=2,
        Material=3,
        ViewSpacePosition=4,
        Depth,
        Phong
    };

    void setRenderMode(RenderMode newMode);
    void drawScene(Scene *scene);
    PerformanceChart* getLogger();

    void updateFramebuffeSize(int width, int height);

private:
    enum UniformMode{
        PhongUniforms,
        LightingPassUniforms,
        GeometryPassUniforms
    };

    void setUpUniforms(QOpenGLShaderProgram *prog, UniformMode uniformMode);

    const int scr_size = 512;

    GLuint fbo;
    GLuint ntcTex;
    GLuint depthTex;
    GLuint matTex;

    GLuint quadVAO;
    GLuint quadVBO;
    GLuint quadIBO;

    PerformanceChart *performanceLogger;
    long frameCounter;
    static const auto NumQueries = 3;
    bool useQueryB;
    std::vector<GLuint> queryObjectsA;
    std::vector<GLuint64> queryResultsA;
    std::vector<GLuint> queryObjectsB;
    std::vector<GLuint64> queryResultsB;

    QOpenGLShaderProgram *phongProgram;
    QOpenGLShaderProgram *deferredGeomPassProgram;
    QOpenGLShaderProgram *deferredLightingPassProgram;
    QOpenGLShaderProgram *terrainProgram;

    RenderMode mode;
    void setUpFBOTextures();
    void setUpQuad();
    void queryTime(int index);
    void logTimes();
};

#endif // SCENERENDERER_H
