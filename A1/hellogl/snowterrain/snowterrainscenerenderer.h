#ifndef SNOWTERRAINSCENERENDERER_H
#define SNOWTERRAINSCENERENDERER_H

#include <terrain/terrainscenerenderer.h>
#include "snowterrainscene.h"
#include "snowfallrenderer.h"

class SnowTerrainSceneRenderer : public TerrainSceneRenderer
{
public:

    void initGL();
    void drawScene(SnowTerrainScene *scene);

protected:
    void loadShader();
    void drawTerrain(SnowTerrainScene *scene);
    void executeLODCompute(SnowTerrainScene *scene, OpenGLFunctions *f);
    void drawGeometryTrees(OpenGLFunctions *f, SnowTerrainScene *scene);
    void drawImpostorTrees(OpenGLFunctions *f, SnowTerrainScene *scene);
    void drawParticles(SnowTerrainScene *scene);

    QOpenGLShaderProgram *particleProgram;
    SnowFallRenderer particleRenderer;
};

#endif // SNOWTERRAINSCENERENDERER_H
