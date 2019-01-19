#ifndef SNOWTERRAINSCENERENDERER_H
#define SNOWTERRAINSCENERENDERER_H

#include <terrain/terrainscenerenderer.h>
#include "snowterrainscene.h"

class SnowTerrainSceneRenderer : public TerrainSceneRenderer
{
public:

    void initGL();
    void drawScene(SnowTerrainScene *scene);

protected:
    void loadShader();
    void drawTerrain(SnowTerrainScene *scene);

    QOpenGLShaderProgram *snowTerrainProgram;
};

#endif // SNOWTERRAINSCENERENDERER_H
