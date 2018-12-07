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
    OpenGLFunctions *f = OpenGLFunctions::instance();

    f->glGenBuffers(1, &treeDataGeometryBuffer);
    f->glBindBuffer(GL_SHADER_STORAGE_BUFFER, treeDataGeometryBuffer);
    f->glBufferData(GL_SHADER_STORAGE_BUFFER, 4000000 * sizeof(GLfloat), NULL, GL_STATIC_DRAW);

    f->glGenBuffers(1, &treeDataImpostorBuffer);
    f->glBindBuffer(GL_SHADER_STORAGE_BUFFER, treeDataImpostorBuffer);
    f->glBufferData(GL_SHADER_STORAGE_BUFFER, 4000000 * sizeof(GLfloat), NULL, GL_STATIC_DRAW);

    f->glGenBuffers(1, &drawCommandBuffer);
    f->glBindBuffer(GL_SHADER_STORAGE_BUFFER, drawCommandBuffer);
    f->glBufferData(GL_SHADER_STORAGE_BUFFER, 4000000 * sizeof(GLfloat), NULL, GL_STATIC_DRAW);

    GLuint indexBuffer;
    f->glGenBuffers(1, &indexBuffer);
    f->glBindBuffer(GL_SHADER_STORAGE_BUFFER, indexBuffer);
    f->glBufferData(GL_SHADER_STORAGE_BUFFER, 3 * sizeof(GLuint), NULL, GL_STATIC_DRAW);

    f->glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, treeDataGeometryBuffer);
    f->glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, treeDataImpostorBuffer);
    f->glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, drawCommandBuffer);
    f->glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, indexBuffer);
}

void TerrainSceneRenderer::createImpostorObjects()
{
    OpenGLFunctions *f = OpenGLFunctions::instance();

    f->glGenFramebuffers(1, &impostorFBO);
    f->glBindFramebuffer(GL_FRAMEBUFFER, impostorFBO);

    f->glGenTextures(1, &impostorTex);
    f->glBindTexture(GL_TEXTURE_2D_ARRAY, impostorTex);
    f->glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    f->glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    f->glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    f->glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    f->glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA8,
                    640, 640, numImpostorImages,
                    0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    f->glBindFramebuffer(GL_FRAMEBUFFER, 0);
    f->glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
}

void TerrainSceneRenderer::initGL() {

    SceneRenderer::init();

    OpenGLFunctions *f = OpenGLFunctions::instance();



    loadShader();

    setUpTreeBuffers();
    createImpostorObjects();

    f->glBindFramebuffer(GL_FRAMEBUFFER, impostorFBO);
    f->glBindTexture(GL_TEXTURE_2D_ARRAY, impostorTex);
    phongProgram->bind();
    for(int i = 0; i < numImpostorImages; i++) {
        f->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        f->glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_ARRAY, impostorTex, i);

    }
}

void TerrainSceneRenderer::drawScene(TerrainScene *scene) {

    OpenGLFunctions *f = OpenGLFunctions::instance();

    if(!scene->wasLoaded)
        return;

    f->glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, scene->forrest.getTreeDataBuffer());

    f->glBindBuffer(GL_SHADER_STORAGE_BUFFER, treeDataGeometryBuffer);
    f->glBufferData(GL_SHADER_STORAGE_BUFFER, 4000000 * sizeof(GLfloat), NULL, GL_STATIC_DRAW);

    f->glBindBuffer(GL_SHADER_STORAGE_BUFFER, treeDataImpostorBuffer);
    f->glBufferData(GL_SHADER_STORAGE_BUFFER, 4000000 * sizeof(GLfloat), NULL, GL_STATIC_DRAW);

    f->glBindBuffer(GL_SHADER_STORAGE_BUFFER, drawCommandBuffer);
    f->glBufferData(GL_SHADER_STORAGE_BUFFER, 4000000 * sizeof(GLfloat), NULL, GL_STATIC_DRAW);

    treeDataProgram->bind();
    std::vector<GLuint> vertexCounts = scene->tree.getVertexCounts(20);
    treeDataProgram->setUniformValue("maxGeomTreeDistance", (GLfloat) scene->forrest.getMaxGeometryDistance());
    GLint arrayLoc = treeDataProgram->uniformLocation("vertexCount");
    f->glUniform1uiv(arrayLoc, 20,  reinterpret_cast<GLuint *>(&vertexCounts.at(0)));

    scene->setUpCameraUniforms(treeDataProgram);
    GLint workGroupSize[3];
    f->glGetProgramiv(treeDataProgram->programId(), GL_COMPUTE_WORK_GROUP_SIZE, workGroupSize);
    GLint numInvocations = scene->forrest.getNumTrees()/workGroupSize[0] + 1;
    f->glDispatchCompute(numInvocations, 1, 1);
    f->glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

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

    f->glBindBuffer(GL_DRAW_INDIRECT_BUFFER, drawCommandBuffer);

    scene->setUpUniforms(treeProgram, true);
    scene->setUpCameraUniforms(treeProgram);
    scene->terrain.setHeightMapUniform(treeProgram);

    Tree* tree = scene->forrest.getTree();
    for(int i =0; i< tree->meshes.size(); i++) {

        f->glBindVertexArray(tree->meshes[i]->vao);
        f->glBindBuffer(GL_DRAW_INDIRECT_BUFFER, drawCommandBuffer);

        f->glBindBuffer(GL_ARRAY_BUFFER, treeDataGeometryBuffer);
        f->glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 0, 0);
        f->glEnableVertexAttribArray(4);
        f->glVertexAttribDivisor(4, 1);

        treeProgram->setUniformValue("materialIndex", tree->meshes[i]->materialIndex);
        f->glDrawElementsIndirect(GL_TRIANGLES, scene->forrest.getTree()->meshes[i]->index_type, (void*)((i+1) * 5 * sizeof(GLuint)));
        f->glBindVertexArray(0);
    }

    queryTime(2);

    logTimes();

    useQueryB = !useQueryB;
    frameCounter++;
}
