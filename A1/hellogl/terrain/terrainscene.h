#ifndef TERRAINSCENE_H
#define TERRAINSCENE_H

#include <QFile>

#include "scenegraph/scene.h"
#include "terrain.h"
#include "forrest.h"
#include "tree.h"
#include "forrestdata.h"

#include "tiny_gltf.h"

class TerrainScene : public Scene
{
    Q_OBJECT

public:
    TerrainScene();

    void drawScene(QOpenGLShaderProgram *terrainShader, QOpenGLShaderProgram *treeShader, bool setUpUniforms);

    void setHeightScaling(float scaling);
    void drawTerrain(QOpenGLShaderProgram *terrainProg);
    void drawForrest(QOpenGLShaderProgram *treeProg);
    void loadTerrain(QFile *pgmFile);
    void loadTree(tinygltf::Model gltf_model);
    void setUpForrest();

    float setUpCameraUniforms(QOpenGLShaderProgram *terrainProg);

    float terrainHeightScaling;
    float terrainWidthScaling;

    Terrain terrain;
    Forrest forrest;
    Tree tree;

public slots:
    void changeForrestParameter(ForrestData data);

};

#endif // TERRAINSCENE_H
