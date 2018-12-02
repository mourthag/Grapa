#include "terrainscene.h"

void TerrainScene::loadTerrain(QFile *pgmFile) {
    terrain.loadFromFile(pgmFile);
}

void TerrainScene::setHeightScaling(float scaling) {
    terrainHeightScaling = 1.0 / scaling;
}

void TerrainScene::drawForrest(QOpenGLShaderProgram *treeProg) {

    setUpUniforms(treeProg, false);
    QVector3D camPos = camera.position();
    camPos.setY(0);
    treeProg->setUniformValue("camPos", camPos);
    treeProg->setUniformValue("normalMat", camera.viewMatrix().normalMatrix());
    treeProg->setUniformValue("heightScaling", terrainHeightScaling);

    for(int i=0; i < forrests.size(); i++) {
        forrests[i].draw(treeProg);
    }
}

void TerrainScene::drawTerrain(QOpenGLShaderProgram *terrainProg) {
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
