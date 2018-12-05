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

private:
    GLuint treeDataImpostorBuffer;
    GLuint treeDataGeometryBuffer;
    GLuint drawCommandImpostorBuffer;
    GLuint drawCommandGeometryBuffer;

    QOpenGLShaderProgram *terrainProgram;
    QOpenGLShaderProgram *treeDataProgram;
    QOpenGLShaderProgram *treeProgram;
    void loadShader();
    void setUpTreeBuffers();
};

#endif // TERRAINSCENERENDERER_H
