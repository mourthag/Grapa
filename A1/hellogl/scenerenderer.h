#ifndef SCENERENDERER_H
#define SCENERENDERER_H

#include <QOpenGLFunctions_4_0_Core>
#include <QDebug>
#include <QOpenGLShaderProgram>

#include "scene.h"


class SceneRenderer : QOpenGLFunctions_4_0_Core
{
public:
    SceneRenderer();
    void initGL();

    enum RenderMode {
        Phong,
        Deferred,
        Material,
        Depth,
        Normal,
        ViewSpacePosition
    };

    void setRenderMode(RenderMode newMode);
    void drawScene(Scene *scene, QMatrix4x4 *viewMat);

private:
    enum UniformMode{
        PhongUniforms,
        LightingPassUniforms,
        GeometryPassUniforms
    };

    void setUpUniforms(QOpenGLShaderProgram *prog, UniformMode uniformMode);

    const int scr_size = 1024;

    GLuint fbo;
    GLuint ntcTex;
    GLuint depthTex;
    GLuint matTex;

    GLuint quadVAO;
    GLuint quadVBO;
    GLuint quadIBO;

    QOpenGLShaderProgram *phongProgram;
    QOpenGLShaderProgram *deferredGeomPassProgram;
    QOpenGLShaderProgram *deferredLightingPassProgram;

    RenderMode mode;
    void setUpFBOTextures();
    void setUpQuad();
};

#endif // SCENERENDERER_H
