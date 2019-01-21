#ifndef SNOWTERRAINSCENE_H
#define SNOWTERRAINSCENE_H

#include <terrain/terrainscene.h>
#include "snowterrain.h"

class SnowTerrainScene : public TerrainScene
{
public:
    SnowTerrain snowTerrain;

    void initGL();
    void loadTerrain(QFile *pgmFile);
    void drawTerrain(QOpenGLShaderProgram *snowTerrainProg);
    void drawForrest(QOpenGLShaderProgram *treeProg);
    void setUpForrest();

public slots:
    void changeForrestParameter(ForrestData data);

};

#endif // SNOWTERRAINSCENE_H
