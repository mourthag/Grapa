#include "terrainscenerenderer.h"

TerrainSceneRenderer::TerrainSceneRenderer() {

}

void TerrainSceneRenderer::initGL() {
    SceneRenderer::initGL();

    terrainProgram = new QOpenGLShaderProgram();
    terrainProgram->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shader/terrainvertshader.vert");
    terrainProgram->addShaderFromSourceFile(QOpenGLShader::TessellationControl, ":/shader/terraintesselationcontrolshader.tcs");
    terrainProgram->addShaderFromSourceFile(QOpenGLShader::TessellationEvaluation, ":/shader/terraintesselationevalshader.tes");
    terrainProgram->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shader/terrainfragshader.frag");
    terrainProgram->link();

    treeProgram = new QOpenGLShaderProgram();
    treeProgram->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shader/treevertshader.vert");
    treeProgram->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shader/treefragshader.frag");
    treeProgram->link();

}

void TerrainSceneRenderer::drawScene(TerrainScene *scene) {

    queryTime(0);
    terrainProgram->bind();
    terrainProgram->setUniformValue("tessLevel", tesselation);
    setUpUniforms(terrainProgram, PhongUniforms);
    scene->drawTerrain(terrainProgram);
    queryTime(1);
    treeProgram->bind();
    setUpUniforms(treeProgram, PhongUniforms);
    scene->drawForrest(treeProgram);
    queryTime(2);

    logTimes();

    useQueryB = !useQueryB;
    frameCounter++;
}
