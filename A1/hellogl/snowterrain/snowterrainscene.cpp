#include "snowterrainscene.h"

void SnowTerrainScene::initGL() {
    snowTerrain.initGL();

    QTimer *snowTimer = new QTimer();
    QObject::connect(snowTimer, &QTimer::timeout, [ = ]() {
        snowTerrain.replenishSnow();
        snowTerrain.updateTexture(QVector2D(camera.position()), 4);
    } );
    snowTimer->start(10000);
}

void SnowTerrainScene::loadTerrain(QFile *pgmFile) {
    snowTerrain.loadFromFile(pgmFile);
}

void SnowTerrainScene::changeForrestParameter(ForrestData data) {

    forrest.setData(data, &snowTerrain);
}

void SnowTerrainScene::setUpForrest() {
    forrest.generateTreeData(&snowTerrain);

}
void SnowTerrainScene::drawForrest(QOpenGLShaderProgram *treeProg) {

    if(!wasLoaded)
            return;

    setUpUniforms(treeProg, true);
    setUpCameraUniforms(treeProg);
    snowTerrain.setHeightMapUniform(treeProg);
    forrest.draw(treeProg);
}

void SnowTerrainScene::drawTerrain(QOpenGLShaderProgram *snowTerrainProg) {
    if(!wasLoaded)
        return;

    setUpUniforms(snowTerrainProg, false);
    float cameraHeight = setUpCameraUniforms(snowTerrainProg);

    QVector3D camPos = camera.position();
    QVector2D camGridPos(camPos.x(), camPos.z());
    float height = snowTerrain.getHeight(camGridPos)*terrainHeightScaling;
    camPos.setY(std::max(height +2, cameraHeight));
    camera.setPosition(camPos);
    snowTerrain.drawTerrain(snowTerrainProg, camPos);
}
