#include "snowterrainscenerenderer.h"

void SnowTerrainSceneRenderer::loadShader() {


    terrainProgram = new QOpenGLShaderProgram();
    terrainProgram->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shader/snowterrain/snowterrainvertshader.vert");
    terrainProgram->addShaderFromSourceFile(QOpenGLShader::TessellationControl, ":/shader/snowterrain/snowterraintesselationcontrolshader.tcs");
    terrainProgram->addShaderFromSourceFile(QOpenGLShader::TessellationEvaluation, ":/shader/snowterrain/snowterraintesselationevalshader.tes");
    terrainProgram->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shader/snowterrain/snowterrainfragshader.frag");
    terrainProgram->link();

    treeDataProgram = new QOpenGLShaderProgram();
    treeDataProgram->addShaderFromSourceFile(QOpenGLShader::Compute, ":/shader/terrain/treecompshader.comp");
    treeDataProgram->link();

    treeProgram = new QOpenGLShaderProgram();
    treeProgram->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shader/snowterrain/snowtreevertshader.vert");
    treeProgram->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shader/snowterrain/snowtreefragshader.frag");
    treeProgram->link();

    treeImpostorProgram = new QOpenGLShaderProgram();
    treeImpostorProgram->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shader/terrain/treeimpostorvertshader.vert");
    treeImpostorProgram->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shader/terrain/treeimpostorfragshader.frag");
    treeImpostorProgram->link();

    skyboxProgram = new QOpenGLShaderProgram();
    skyboxProgram->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shader/skybox/skyboxvertshader.vert");
    skyboxProgram->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shader/skybox/skyboxfragshader.frag");
    skyboxProgram->link();

    particleProgram = new QOpenGLShaderProgram();
    particleProgram->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shader/snowterrain/snowparticlevertshader.vert");
    particleProgram->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shader/snowterrain/snowparticlefragshader.frag");
    particleProgram->link();
}

void SnowTerrainSceneRenderer::initGL() {

    SceneRenderer::init();

    loadShader();

    setUpTreeBuffers();
    setUpSkyBox();

    particleRenderer.initGL();
    particleRenderer.createRandomParticles();
    particleRenderer.updateBuffer();
}

void SnowTerrainSceneRenderer::executeLODCompute(SnowTerrainScene *scene, OpenGLFunctions *f) {
    f->glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, scene->forrest.getTreeDataBuffer());

    f->glBindBuffer(GL_SHADER_STORAGE_BUFFER, treeDataGeometryBuffer);
    f->glBufferData(GL_SHADER_STORAGE_BUFFER, scene->forrest.getNumTrees() * 4 * sizeof(GLfloat), NULL, GL_STATIC_DRAW);

    f->glBindBuffer(GL_SHADER_STORAGE_BUFFER, treeDataImpostorBuffer);
    f->glBufferData(GL_SHADER_STORAGE_BUFFER, scene->forrest.getNumTrees() * 4 * sizeof(GLfloat), NULL, GL_STATIC_DRAW);

    f->glBindBuffer(GL_SHADER_STORAGE_BUFFER, drawCommandBuffer);
    f->glBufferData(GL_SHADER_STORAGE_BUFFER, 1000 * sizeof(GLfloat), NULL, GL_STATIC_DRAW);
    treeDataProgram->bind();

    std::vector<GLuint> vertexCounts = scene->tree.getVertexCounts(20);
    GLint arrayLoc = treeDataProgram->uniformLocation("vertexCount");
    f->glUniform1uiv(arrayLoc, 20,  reinterpret_cast<GLuint *>(&vertexCounts.at(0)));

    treeDataProgram->setUniformValue("frustumCullingEnabled", frustumCullingEnabled);
    treeDataProgram->setUniformValue("boundingSphere", scene->tree.boundingSphere);
    std::vector<QVector4D> *planes = &scene->getCamera()->frustumPlanes;

    treeDataProgram->setUniformValue("frustumRight", planes->at(0));
    treeDataProgram->setUniformValue("frustumLeft", planes->at(1));
    treeDataProgram->setUniformValue("frustumTop", planes->at(2));
    treeDataProgram->setUniformValue("frustumBottom", planes->at(3));
    treeDataProgram->setUniformValue("frustumNear", planes->at(4));
    treeDataProgram->setUniformValue("frustumFar", planes->at(5));

    treeDataProgram->setUniformValue("maxGeomTreeDistance", (GLfloat) scene->forrest.getMaxGeometryDistance());
    treeDataProgram->setUniformValue("maxImpostorTreeDistance", (GLfloat) scene->forrest.getMaxImpostorDistance());

    scene->setUpCameraUniforms(treeDataProgram);
    scene->snowTerrain.setHeightMapUniform(treeDataProgram);

    scene->setUpCameraUniforms(treeDataProgram);
    GLint workGroupSize[3];
    f->glGetProgramiv(treeDataProgram->programId(), GL_COMPUTE_WORK_GROUP_SIZE, workGroupSize);
    GLint numInvocations = scene->forrest.getNumTrees()/workGroupSize[0] + 1;
    f->glDispatchCompute(numInvocations, 1, 1);
    f->glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

}

void SnowTerrainSceneRenderer::drawGeometryTrees(OpenGLFunctions *f, SnowTerrainScene *scene) {
    treeProgram->bind();
    setUpUniforms(treeProgram, PhongUniforms);
    QMatrix4x4 test;
    test.rotate(-90, 1, 0, 0);
    treeProgram->setUniformValue("modelMat", test);

    f->glBindBuffer(GL_DRAW_INDIRECT_BUFFER, drawCommandBuffer);

    scene->setUpUniforms(treeProgram, true);
    scene->setUpCameraUniforms(treeProgram);
    scene->snowTerrain.setSnowUniforms(treeProgram);
    scene->snowTerrain.setHeightMapUniform(treeProgram);

    Tree* tree = &scene->tree;
    for(int i =0; i< tree->meshes.size(); i++) {

        f->glBindVertexArray(tree->meshes[i]->vao);
        f->glBindBuffer(GL_DRAW_INDIRECT_BUFFER, drawCommandBuffer);

        f->glBindBuffer(GL_ARRAY_BUFFER, treeDataGeometryBuffer);
        f->glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 0, 0);
        f->glEnableVertexAttribArray(4);
        f->glVertexAttribDivisor(4, 1);

        treeProgram->setUniformValue("materialIndex", tree->meshes[i]->materialIndex);
        f->glDrawElementsIndirect(GL_TRIANGLES, scene->tree.meshes[i]->index_type, (void*)((i+1) * 5 * sizeof(GLuint)));
        f->glBindVertexArray(0);
    }
}

void SnowTerrainSceneRenderer::drawImpostorTrees(OpenGLFunctions *f, SnowTerrainScene *scene) {
    treeImpostorProgram->bind();
    scene->setUpUniforms(treeImpostorProgram, false);
    scene->setUpCameraUniforms(treeImpostorProgram);
    scene->snowTerrain.setHeightMapUniform(treeImpostorProgram);

    treeImpostorProgram->setUniformValue("impostorTextures", 6);
    f->glActiveTexture(GL_TEXTURE6);
    f->glBindTexture(GL_TEXTURE_2D_ARRAY, impostorTex);
    treeImpostorProgram->setUniformValue("numImpostors", numImpostorImages);

    f->glBindVertexArray(quadVAO);
    f->glBindBuffer(GL_DRAW_INDIRECT_BUFFER, drawCommandBuffer);

    f->glBindBuffer(GL_ARRAY_BUFFER, treeDataImpostorBuffer);
    f->glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 0, 0);
    f->glEnableVertexAttribArray(4);
    f->glVertexAttribDivisor(4, 1);

    f->glDrawElementsIndirect(GL_TRIANGLE_STRIP, GL_UNSIGNED_INT, (void*)0);
    f->glBindVertexArray(0);
}

void SnowTerrainSceneRenderer::drawTerrain(SnowTerrainScene *scene) {

    terrainProgram->bind();
    terrainProgram->setUniformValue("tessLevel", tesselation);
    setUpUniforms(terrainProgram, PhongUniforms);
    scene->drawTerrain(terrainProgram);
}

void SnowTerrainSceneRenderer::drawParticles(SnowTerrainScene *scene) {
    particleProgram->bind();

    scene->setUpCameraUniforms(particleProgram);
    scene->setUpUniforms(particleProgram, false);

    particleRenderer.setHeight(scene->getCamera()->position().y(), scene->snowTerrain.getMinimumSnowHeight(), 50.0);
    particleRenderer.drawParticles(particleProgram);
}

SnowFallRenderer *SnowTerrainSceneRenderer::getParticleRenderer()
{
    return &particleRenderer;
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

    drawParticles(scene);
    queryTime(6);

    logTimes();

    useQueryB = !useQueryB;
    frameCounter++;

}
