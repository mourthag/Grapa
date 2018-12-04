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
    QVector3D camPos = camera.position();
    camPos.setY(0);
    treeProg->setUniformValue("camPos", camPos);
    treeProg->setUniformValue("normalMat", camera.viewMatrix().normalMatrix());
    treeProg->setUniformValue("heightScaling", terrainHeightScaling);

    forrest.draw(treeProg);
}

void TerrainScene::drawTerrain(QOpenGLShaderProgram *terrainProg) {
    if(!wasLoaded)
        return;

    setUpUniforms(terrainProg, false);
    QVector3D camPos = camera.position();
    float cameraHeight = camPos.y();
    camPos.setY(0);
    terrainProg->setUniformValue("camPos", camPos);
    terrainProg->setUniformValue("normalMat", camera.viewMatrix().normalMatrix());
    terrainProg->setUniformValue("heightScaling", terrainHeightScaling);

    QVector2D camGridPos(camPos.x(), camPos.z());
    float height = terrain.getHeight(camGridPos)*terrainHeightScaling;
    camPos.setY(std::min(-height -2, cameraHeight));
    camera.setPosition(camPos);
    terrain.drawTerrain(terrainProg, camPos);
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
    forrest = Forrest(1000, 20, 0.5, 0.8, 1.4, &terrain);
}
