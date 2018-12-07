#include "terrainscene.h"

TerrainScene::TerrainScene() {
}


void TerrainScene::loadTerrain(QFile *pgmFile) {
    terrain.loadFromFile(pgmFile);
}

void TerrainScene::setHeightScaling(float scaling) {
    terrainHeightScaling = 1.0 / scaling;
}

void TerrainScene::drawForrest(QOpenGLShaderProgram *treeProg) {

    if(!wasLoaded)
            return;

    setUpUniforms(treeProg, true);
    setUpCameraUniforms(treeProg);
    terrain.setHeightMapUniform(treeProg);
    forrest.draw(treeProg);
}

float TerrainScene::setUpCameraUniforms(QOpenGLShaderProgram *terrainProg)
{
    QVector3D camPos = camera.position();
    float cameraHeight = camPos.y();
    camPos.setY(0);
    terrainProg->setUniformValue("camPos", camPos);
    terrainProg->setUniformValue("normalMat", camera.viewMatrix().normalMatrix());
    terrainProg->setUniformValue("heightScaling", terrainHeightScaling);

    return cameraHeight;
}

void TerrainScene::drawTerrain(QOpenGLShaderProgram *terrainProg) {
    if(!wasLoaded)
        return;

    setUpUniforms(terrainProg, false);
    float cameraHeight = setUpCameraUniforms(terrainProg);

    QVector3D camPos = camera.position();
    QVector2D camGridPos(camPos.x(), camPos.z());
    float height = terrain.getHeight(camGridPos)*terrainHeightScaling;
    camPos.setY(std::max(height +2, cameraHeight));
    camera.setPosition(camPos);
    terrain.drawTerrain(terrainProg, camPos);
}

void TerrainScene::changeForrestParameter(ForrestData data) {
    forrest.setData(data, &terrain);
}

void TerrainScene::loadTree(tinygltf::Model gltf_model) {
    model = gltf_model;

    loadTextures();
    loadMaterials();
    tree.loadTree(&model, 0);

    forrest.setTree(&tree);
    wasLoaded = true;
}

void TerrainScene::setUpForrest() {
    forrest.generateTreeData(&terrain);
}
