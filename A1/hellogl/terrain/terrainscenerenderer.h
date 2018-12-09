#ifndef TERRAINSCENERENDERER_H
#define TERRAINSCENERENDERER_H

#include "scenegraph/scenerenderer.h"
#include "terrainscene.h"
#include <QFile>
#include <QImage>

class TerrainSceneRenderer : public SceneRenderer
{
public:
    TerrainSceneRenderer();

    void initGL();
    void drawScene(TerrainScene *scene);
    void createImpostorTex(TerrainScene *scene, QOpenGLContext *context);
    void createImpostorObjects();
    void loadSkybox(QString dir);

private:
    GLuint skyboxVAO;
    GLuint skyboxVBO;
    GLuint skyboxEBO;
    GLuint impostorFBO;
    GLuint impostorTex;
    int numImpostorImages;

    GLuint skyboxTex;

    GLuint treeDataImpostorBuffer;
    GLuint treeDataGeometryBuffer;
    GLuint drawCommandBuffer;

    QOpenGLShaderProgram *skyboxProgram;
    QOpenGLShaderProgram *terrainProgram;
    QOpenGLShaderProgram *treeDataProgram;
    QOpenGLShaderProgram *treeProgram;
    QOpenGLShaderProgram *treeImpostorProgram;
    void loadShader();
    void setUpTreeBuffers();
    void setUpSkyBox();
};

#endif // TERRAINSCENERENDERER_H
