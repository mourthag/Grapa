#ifndef TERRAINSCENERENDERER_H
#define TERRAINSCENERENDERER_H

#include "scenegraph/scenerenderer.h"
#include "terrainscene.h"

class TerrainSceneRenderer : public SceneRenderer
{
public:
    TerrainSceneRenderer();

    void initGL();
    void drawScene(TerrainScene *scene);
    void createImpostorTex(TerrainScene *scene);

private:
    GLuint impostorFBO;
    GLuint impostorTex;
    int numImpostorImages;

    GLuint treeDataImpostorBuffer;
    GLuint treeDataGeometryBuffer;
    GLuint drawCommandBuffer;

    QOpenGLShaderProgram *terrainProgram;
    QOpenGLShaderProgram *treeDataProgram;
    QOpenGLShaderProgram *treeProgram;
    QOpenGLShaderProgram *treeImpostorProgram;
    void loadShader();
    void setUpTreeBuffers();
    void createImpostorObjects();
};

#endif // TERRAINSCENERENDERER_H
