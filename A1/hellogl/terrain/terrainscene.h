#ifndef TERRAINSCENE_H
#define TERRAINSCENE_H

#include <QFile>

#include "scenegraph/scene.h"
#include "terrain.h"
#include "forrest.h"
#include "tree.h"

#include "tiny_gltf.h"

class TerrainScene : Scene
{
public:
    void drawScene(QOpenGLShaderProgram *terrainShader, QOpenGLShaderProgram *treeShader, bool setUpUniforms);

    void setHeightScaling(float scaling);
    void drawTerrain(QOpenGLShaderProgram *terrainProg);
    void drawForrest(QOpenGLShaderProgram *treeProg);
    void loadTerrain(QFile *pgmFile);
    void loadTree(tinygltf::Model gltf_model);

private:
    float terrainHeightScaling;
    float terrainWidthScaling;

    Terrain terrain;
    std::vector<Forrest> forrests;
    std::vector<Tree> trees;

};

#endif // TERRAINSCENE_H
