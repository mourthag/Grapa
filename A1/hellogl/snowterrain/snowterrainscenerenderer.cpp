#include "snowterrainscenerenderer.h"

void SnowTerrainSceneRenderer::loadShader() {
    TerrainSceneRenderer::loadShader();

    snowTerrainProgram = new QOpenGLShaderProgram();
    snowTerrainProgram->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shader/snowterrain/snowterrainvertshader.vert");
    snowTerrainProgram->addShaderFromSourceFile(QOpenGLShader::TessellationControl, ":/shader/snowterrain/snowterraintesselationcontrolshader.tcs");
    snowTerrainProgram->addShaderFromSourceFile(QOpenGLShader::TessellationEvaluation, ":/shader/snowterrain/snowterraintesselationevalshader.tes");
    snowTerrainProgram->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shader/snowterrain/snowterrainfragshader.frag");
    snowTerrainProgram->link();

}

void SnowTerrainSceneRenderer::initGL() {

    SceneRenderer::init();

    loadShader();

    setUpTreeBuffers();
    setUpSkyBox();
}

void SnowTerrainSceneRenderer::drawTerrain(SnowTerrainScene *scene) {

    snowTerrainProgram->bind();
    snowTerrainProgram->setUniformValue("tessLevel", tesselation);
    setUpUniforms(snowTerrainProgram, PhongUniforms);
    scene->drawTerrain(snowTerrainProgram);
}

void SnowTerrainSceneRenderer::drawScene(SnowTerrainScene *scene) {
    OpenGLFunctions *f = OpenGLFunctions::instance();

    if(!scene->wasLoaded)
        return;

    queryTime(0);

    executeLODCompute(scene, f);
    queryTime(1);

    if(terrainDrawEnabled)
        drawTerrain(scene);
    queryTime(2);

    if(treeDrawEnabled)
        drawGeometryTrees(f, scene);
    queryTime(3);

    if(treeDrawEnabled)
        drawImpostorTrees(f, scene);
    queryTime(4);

    if(skyboxDrawEnabled)
        drawSkybox(scene, f);
    queryTime(5);

    logTimes();

    useQueryB = !useQueryB;
    frameCounter++;

}
