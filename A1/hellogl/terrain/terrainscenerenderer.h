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
    void setNumImpostors(int val);
    void createImpostorTex(TerrainScene *scene);
    void createImpostorObjects();
    void loadSkybox(QString dir);

    bool frustumCullingEnabled;
    bool terrainDrawEnabled;
    bool treeDrawEnabled;
    bool skyboxDrawEnabled;

protected:
    GLuint skyboxVAO;
    GLuint skyboxVBO;
    GLuint skyboxEBO;
    GLuint impostorFBO;
    GLuint impostorTempTex;
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
    void executeLODCompute(TerrainScene *scene, OpenGLFunctions *f);
    void drawTerrain(TerrainScene *scene);
    void drawGeometryTrees(OpenGLFunctions *f, TerrainScene *scene);
    void drawImpostorTrees(OpenGLFunctions *f, TerrainScene *scene);
    void drawSkybox(TerrainScene *scene, OpenGLFunctions *f);

    const int impostorTexSize = 640;
};

#endif // TERRAINSCENERENDERER_H
