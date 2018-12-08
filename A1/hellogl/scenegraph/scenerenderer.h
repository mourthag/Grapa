#ifndef SCENERENDERER_H
#define SCENERENDERER_H

#include <QDebug>
#include <QOpenGLShaderProgram>

#include "scene.h"
#include "light.h"
#include "util/performancechart.h"


class SceneRenderer
{
public:
    SceneRenderer();
    void init();

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
    Light* getLight();

    void setTesselation(int t);
    void updateFramebuffeSize(int width, int height);

protected:
    enum UniformMode{
        PhongUniforms,
        TerrainUniforms,
        LightingPassUniforms,
        GeometryPassUniforms
    };

    void setUpUniforms(QOpenGLShaderProgram *prog, UniformMode uniformMode);

    int tesselation;

    Light light;

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
    static const auto NumQueries = 6;
    bool useQueryB;
    std::vector<GLuint> queryObjectsA;
    std::vector<GLuint64> queryResultsA;
    std::vector<GLuint> queryObjectsB;
    std::vector<GLuint64> queryResultsB;

    QOpenGLShaderProgram *phongProgram;
    QOpenGLShaderProgram *deferredGeomPassProgram;
    QOpenGLShaderProgram *deferredLightingPassProgram;

    RenderMode mode;
    void setUpFBOTextures();
    void setUpQuad();
    void queryTime(int index);
    void logTimes();
};

#endif // SCENERENDERER_H
