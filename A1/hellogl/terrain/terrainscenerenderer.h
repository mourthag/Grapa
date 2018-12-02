#ifndef TERRAINSCENERENDERER_H
#define TERRAINSCENERENDERER_H

#include "scenegraph/scenerenderer.h"

class TerrainSceneRenderer : public SceneRenderer
{
public:
    TerrainSceneRenderer();
    void drawScene(Scene *scene);
};

#endif // TERRAINSCENERENDERER_H
