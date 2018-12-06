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
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, treeDataGeometryBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, 4000000 * sizeof(GLfloat), NULL, GL_STATIC_DRAW);

    glGenBuffers(1, &treeDataImpostorBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, treeDataImpostorBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, 4000000 * sizeof(GLfloat), NULL, GL_STATIC_DRAW);

    glGenBuffers(1, &drawCommandBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, drawCommandBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, 4000000 * sizeof(GLfloat), NULL, GL_STATIC_DRAW);

    GLuint bufferTest;
    glGenBuffers(1, &bufferTest);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, bufferTest);
    glBufferData(GL_SHADER_STORAGE_BUFFER, 3 * sizeof(GLuint), NULL, GL_STATIC_DRAW);

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, treeDataGeometryBuffer);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, treeDataImpostorBuffer);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, drawCommandBuffer);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, bufferTest);
}

void TerrainSceneRenderer::initGL() {
    SceneRenderer::initGL();

    loadShader();

    setUpTreeBuffers();

}

void TerrainSceneRenderer::drawScene(TerrainScene *scene) {

    if(!scene->wasLoaded)
        return;

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, scene->forrest.getTreeDataBuffer());

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, treeDataGeometryBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, 4000000 * sizeof(GLfloat), NULL, GL_STATIC_DRAW);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, treeDataImpostorBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, 4000000 * sizeof(GLfloat), NULL, GL_STATIC_DRAW);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, drawCommandBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, 4000000 * sizeof(GLfloat), NULL, GL_STATIC_DRAW);

    treeDataProgram->bind();
    std::vector<GLuint> vertexCounts = scene->tree.getVertexCounts(20);

    treeDataProgram->setUniformValue("maxGeomTreeDistance", (GLfloat) 120.0);
    GLint arrayLoc = treeDataProgram->uniformLocation("vertexCount");
    glUniform1uiv(arrayLoc, 20,  reinterpret_cast<GLuint *>(&vertexCounts.at(0)));

    scene->setUpCameraUniforms(treeDataProgram);
    GLint workGroupSize[3];
    glGetProgramiv(treeDataProgram->programId(), GL_COMPUTE_WORK_GROUP_SIZE, workGroupSize);
    GLint numInvocations = scene->forrest.treeAmount/workGroupSize[0] + 1;
    glDispatchCompute(numInvocations, 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

    queryTime(0);
    terrainProgram->bind();
    terrainProgram->setUniformValue("tessLevel", tesselation);
    setUpUniforms(terrainProgram, PhongUniforms);
    scene->drawTerrain(terrainProgram);
    queryTime(1);
    treeProgram->bind();
    setUpUniforms(treeProgram, PhongUniforms);
    QMatrix4x4 test;
    test.rotate(-90, 1, 0, 0);
    treeProgram->setUniformValue("modelMat", test);

    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, drawCommandBuffer);

    scene->setUpUniforms(treeProgram, true);
    scene->setUpCameraUniforms(treeProgram);
    scene->terrain.setHeightMapUniform(treeProgram);

    Tree* tree = scene->forrest.getTree();
    for(int i =0; i< tree->meshes.size(); i++) {

        glBindVertexArray(tree->meshes[i]->vao);
        glBindBuffer(GL_DRAW_INDIRECT_BUFFER, drawCommandBuffer);

        glBindBuffer(GL_ARRAY_BUFFER, treeDataGeometryBuffer);
        glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(4);
        glVertexAttribDivisor(4, 1);

        treeProgram->setUniformValue("materialIndex", tree->meshes[i]->materialIndex);
        glDrawElementsIndirect(GL_TRIANGLES, scene->forrest.getTree()->meshes[i]->index_type, (void*)((i+1) * 5 * sizeof(GLuint)));
        glBindVertexArray(0);
    }



    queryTime(2);

    useQueryB = !useQueryB;
    frameCounter++;
}
