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
    QOpenGLShaderProgram *terrainProgram;
    QOpenGLShaderProgram *treeProgram;
};

#endif // TERRAINSCENERENDERER_H
