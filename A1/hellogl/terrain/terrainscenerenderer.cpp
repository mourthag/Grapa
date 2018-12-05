#include "terrainscenerenderer.h"

TerrainSceneRenderer::TerrainSceneRenderer() {

}

void TerrainSceneRenderer::loadShader()
{
    terrainProgram = new QOpenGLShaderProgram();
    terrainProgram->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shader/terrain/terrainvertshader.vert");
    terrainProgram->addShaderFromSourceFile(QOpenGLShader::TessellationControl, ":/shader/terrain/terraintesselationcontrolshader.tcs");
    terrainProgram->addShaderFromSourceFile(QOpenGLShader::TessellationEvaluation, ":/shader/terrain/terraintesselationevalshader.tes");
    terrainProgram->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shader/terrain/terrainfragshader.frag");
    terrainProgram->link();

    treeDataProgram = new QOpenGLShaderProgram();
    treeDataProgram->addShaderFromSourceFile(QOpenGLShader::Compute, ":/shader/terrain/treecompshader.comp");
    treeDataProgram->link();

    treeProgram = new QOpenGLShaderProgram();
    treeProgram->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shader/terrain/treevertshader.vert");
    treeProgram->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shader/terrain/treefragshader.frag");
    treeProgram->link();
}

void TerrainSceneRenderer::setUpTreeBuffers()
{
    glGenBuffers(1, &treeDataGeometryBuffer);
    glGenBuffers(1, &treeDataImpostorBuffer);
    glGenBuffers(1, &drawCommandGeometryBuffer);
    glGenBuffers(1, &drawCommandImpostorBuffer);

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, treeDataGeometryBuffer);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, treeDataImpostorBuffer);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, drawCommandGeometryBuffer);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, drawCommandImpostorBuffer);
}

void TerrainSceneRenderer::initGL() {
    SceneRenderer::initGL();

    loadShader();

    setUpTreeBuffers();

}

void TerrainSceneRenderer::drawScene(TerrainScene *scene) {

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, scene->forrest.getTreeDataBuffer());

    treeDataProgram->bind();
    treeDataProgram->setUniformValue("maxGeomTreeDistance", 1000);
    scene->setUpCameraUniforms(treeDataProgram);
    GLint workGroupSize[3];
    glGetProgramiv(treeDataProgram->programId(), GL_COMPUTE_WORK_GROUP_SIZE, workGroupSize);
    GLint numInvocations = 1000/workGroupSize[1] + 1;
    glDispatchCompute(numInvocations, 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

    GLfloat data[12];
    glBindBuffer(GL_SHADER_STORAGE_BUFFER,scene->forrest.getTreeDataBuffer());
    glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, 12 * sizeof(GLfloat),data );
    for ( int i = 0; i < 12; i ++ ) {
        qDebug() << data[i];
    }
    qDebug() << "-----------------";

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
