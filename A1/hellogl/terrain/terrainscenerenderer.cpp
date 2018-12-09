#include "terrainscenerenderer.h"
static const GLfloat quad_UV[] = {
    0.0, 0.0,
    1.0, 0.0,
    0.0, 1.0,
    1.0, 1.0
};

const GLfloat skybox_vertex[ 72] =
{
    1.0, 1.0, -1.0,
    -1.0 , 1.0 , -1.0 ,
    -1.0 , -1.0 , -1.0 ,
    1.0 , -1.0 , -1.0,
    1.0, 1.0, 1.0,
    -1.0 , 1.0 , 1.0 ,
    -1.0 , -1.0 , 1.0 ,
    1.0 , -1.0 , 1.0,
    1.0, 1.0, -1.0,
    -1.0 , 1.0 , -1.0 ,
    -1.0 , -1.0 , -1.0 ,
    1.0 , -1.0 , -1.0,
    1.0, 1.0, 1.0,
    -1.0 , 1.0 , 1.0 ,
    -1.0 , -1.0 , 1.0 ,
    1.0 , -1.0 , 1.0,
    1.0, 1.0, -1.0,
    -1.0 , 1.0 , -1.0 ,
    -1.0 , -1.0 , -1.0 ,
    1.0 , -1.0 , -1.0,
    1.0, 1.0, 1.0,
    -1.0 , 1.0 , 1.0 ,
    -1.0 , -1.0 , 1.0 ,
    1.0 , -1.0 , 1.0

};

const GLuint skybox_index[ 36] =
{
    //front
    4 , 5 , 6 ,
    4 , 6 , 7 ,
    //behind
    2 , 1 , 0 ,
    3 , 2 , 0 ,
    //top
    8 , 9 , 13 ,
    8 , 13 , 12 ,
    //bottom
    10 , 11 , 14 ,
    15 , 14 , 11 ,
    //right
    16 , 20 , 23 ,
    16 , 23 , 19 ,
    //left
    21 , 17 , 18 ,
    21 , 18 , 22
};

TerrainSceneRenderer::TerrainSceneRenderer() {
    numImpostorImages = 2;

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

    treeImpostorProgram = new QOpenGLShaderProgram();
    treeImpostorProgram->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shader/terrain/treeimpostorvertshader.vert");
    treeImpostorProgram->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shader/terrain/treeimpostorfragshader.frag");
    treeImpostorProgram->link();

    skyboxProgram = new QOpenGLShaderProgram();
    skyboxProgram->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shader/skybox/skyboxvertshader.vert");
    skyboxProgram->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shader/skybox/skyboxfragshader.frag");
    skyboxProgram->link();

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
    f->glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BASE_LEVEL, 0);
    f->glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAX_LEVEL, 0);
    f->glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    f->glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    f->glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA8,
                    640, 640, numImpostorImages,
                    0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);


    f->glBindTexture(GL_TEXTURE_2D, depthTex);
    f->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTex, 0);

    GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};

    f->glDrawBuffers(1, DrawBuffers);

    GLenum fbo_complete = f->glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if(fbo_complete != GL_FRAMEBUFFER_COMPLETE)
        qDebug() << "Framebuffer incomplete!";

    f->glBindFramebuffer(GL_FRAMEBUFFER, 0);
    f->glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
}

void TerrainSceneRenderer::initGL() {

    SceneRenderer::init();

    loadShader();

    setUpTreeBuffers();
    setUpSkyBox();

}

void TerrainSceneRenderer::createImpostorTex(TerrainScene *scene, QOpenGLContext *context) {

    Tree *tree = scene->forrest.getTree();
    OpenGLFunctions *f = OpenGLFunctions::instance();

    f->glBindFramebuffer(GL_FRAMEBUFFER, impostorFBO);
    f->glBindTexture(GL_TEXTURE_2D_ARRAY, impostorTex);
    f->glViewport(0,0,640,640);
    phongProgram->bind();
    QMatrix4x4 projMatrix;
    QVector3D min = scene->tree.boundingBoxMin;
    QVector3D max = scene->tree.boundingBoxMax;
    QMatrix4x4 idMat;
    QMatrix4x4 rotMat;
    rotMat.rotate(-90, 1, 0, 0);
    projMatrix.ortho(min.x(), max.x(), min.y(), max.y(), min.z(), max.z());
    phongProgram->setUniformValue("projMat", projMatrix);
    phongProgram->setUniformValue("modelMat", rotMat);
    phongProgram->setUniformValue("normalMat", rotMat.normalMatrix());
    phongProgram->setUniformValue("viewMat", idMat);
    phongProgram->setUniformValue("lightPos", QVector3D(0,0,-1));
    phongProgram->setUniformValue("lightColor", QVector3D(1,1,1));
    phongProgram->setUniformValue("lightInt", (GLfloat) 1.0);
    for(int i = 0; i < numImpostorImages; i++) {
        f->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        f->glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, impostorTex, 0, i);
        for(int i =0; i< tree->meshes.size(); i++) {

            f->glBindVertexArray(tree->meshes[i]->vao);
            phongProgram->setUniformValue("materialIndex", tree->meshes[i]->materialIndex);
            f->glDrawElements(GL_TRIANGLES, scene->forrest.getTree()->meshes[i]->num_verts, scene->forrest.getTree()->meshes[i]->index_type, (void*) scene->forrest.getTree()->meshes[i]->index_offset);
            f->glBindVertexArray(0);
        }
        context->swapBuffers(context->surface());
        context->makeCurrent(context->surface());
        f->glFinish();
    }
    f->glBindFramebuffer(GL_FRAMEBUFFER, 0);
    f->glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
}

void TerrainSceneRenderer::setUpSkyBox() {

    OpenGLFunctions *f = OpenGLFunctions::instance();
    f->glGenVertexArrays(1, &skyboxVAO);
    f->glBindVertexArray(skyboxVAO);

    f->glGenBuffers(1, &skyboxVBO);
    f->glGenBuffers(1, &skyboxEBO);

    f->glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    f->glBufferData(GL_ARRAY_BUFFER, 72 * sizeof(GLfloat), skybox_vertex, GL_STATIC_DRAW);
    f->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    f->glEnableVertexAttribArray(0);

    f->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, skyboxEBO);
    f->glBufferData(GL_ELEMENT_ARRAY_BUFFER, 36 * sizeof(GLfloat), skybox_index, GL_STATIC_DRAW);

    f->glBindVertexArray(0);

    f->glGenTextures(1, &skyboxTex);
    f->glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTex);
    f->glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    f->glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    f->glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    f->glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    f->glBindTexture(GL_TEXTURE_CUBE_MAP,0);
}

void TerrainSceneRenderer::loadSkybox(QString dir) {
    QImage neg_x(dir + "/teide_rt.jpg");
    QImage neg_y(dir + "/teide_dn.jpg");
    QImage neg_z(dir + "/teide_bk.jpg");
    QImage pos_x(dir + "/teide_lf.jpg");
    QImage pos_y(dir + "/teide_up.jpg");
    QImage pos_z(dir + "/teide_ft.jpg");

    QMatrix rot;
    rot.rotate(90);
    neg_x = neg_x.transformed(rot);
    rot.rotate(90);
    pos_z = pos_z.transformed(rot);
    pos_y = pos_y.transformed(rot);
    rot.rotate(90);
    pos_x = pos_x.transformed(rot);

    OpenGLFunctions *f = OpenGLFunctions::instance();

    f->glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTex);
    f->glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X , 0, GL_RGBA8, pos_x.width(), pos_x.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, pos_x.bits());
    f->glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y , 0, GL_RGBA8, pos_y.width(), pos_y.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, pos_y.bits());
    f->glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z , 0, GL_RGBA8, pos_z.width(), pos_z.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, pos_z.bits());
    f->glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X , 0, GL_RGBA8, neg_x.width(), neg_x.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, neg_x.bits());
    f->glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y , 0, GL_RGBA8, neg_y.width(), neg_y.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, neg_y.bits());
    f->glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z , 0, GL_RGBA8, neg_z.width(), neg_z.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, neg_z.bits());
    f->glBindTexture(GL_TEXTURE_CUBE_MAP,0);
}

void TerrainSceneRenderer::drawScene(TerrainScene *scene) {

    OpenGLFunctions *f = OpenGLFunctions::instance();

    if(!scene->wasLoaded)
        return;

    f->glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, scene->forrest.getTreeDataBuffer());

    f->glBindBuffer(GL_SHADER_STORAGE_BUFFER, treeDataGeometryBuffer);
    f->glBufferData(GL_SHADER_STORAGE_BUFFER, scene->forrest.getNumTrees() * 4 * sizeof(GLfloat), NULL, GL_STATIC_DRAW);

    f->glBindBuffer(GL_SHADER_STORAGE_BUFFER, treeDataImpostorBuffer);
    f->glBufferData(GL_SHADER_STORAGE_BUFFER, scene->forrest.getNumTrees() * 4 * sizeof(GLfloat), NULL, GL_STATIC_DRAW);

    f->glBindBuffer(GL_SHADER_STORAGE_BUFFER, drawCommandBuffer);
    f->glBufferData(GL_SHADER_STORAGE_BUFFER, 1000 * sizeof(GLfloat), NULL, GL_STATIC_DRAW);

    queryTime(0);
    treeDataProgram->bind();
    std::vector<GLuint> vertexCounts = scene->tree.getVertexCounts(20);
    treeDataProgram->setUniformValue("maxGeomTreeDistance", (GLfloat) scene->forrest.getMaxGeometryDistance());
    treeDataProgram->setUniformValue("maxImpostorTreeDistance", (GLfloat) scene->forrest.getMaxImpostorDistance());
    GLint arrayLoc = treeDataProgram->uniformLocation("vertexCount");
    f->glUniform1uiv(arrayLoc, 20,  reinterpret_cast<GLuint *>(&vertexCounts.at(0)));

    scene->setUpCameraUniforms(treeDataProgram);
    GLint workGroupSize[3];
    f->glGetProgramiv(treeDataProgram->programId(), GL_COMPUTE_WORK_GROUP_SIZE, workGroupSize);
    GLint numInvocations = scene->forrest.getNumTrees()/workGroupSize[0] + 1;
    f->glDispatchCompute(numInvocations, 1, 1);
    f->glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

    queryTime(1);
    terrainProgram->bind();
    terrainProgram->setUniformValue("tessLevel", tesselation);
    setUpUniforms(terrainProgram, PhongUniforms);
    scene->drawTerrain(terrainProgram);
    queryTime(2);

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

    queryTime(3);

    treeImpostorProgram->bind();
    scene->setUpUniforms(treeImpostorProgram, false);
    scene->setUpCameraUniforms(treeImpostorProgram);
    scene->terrain.setHeightMapUniform(treeImpostorProgram);

    treeImpostorProgram->setUniformValue("impostorTextures", 6);
    f->glActiveTexture(GL_TEXTURE6);
    f->glBindTexture(GL_TEXTURE_2D_ARRAY, impostorTex);

    f->glBindVertexArray(quadVAO);
    f->glBindBuffer(GL_DRAW_INDIRECT_BUFFER, drawCommandBuffer);

    f->glBindBuffer(GL_ARRAY_BUFFER, treeDataImpostorBuffer);
    f->glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 0, 0);
    f->glEnableVertexAttribArray(4);
    f->glVertexAttribDivisor(4, 1);

    f->glDrawElementsIndirect(GL_TRIANGLE_STRIP, GL_UNSIGNED_INT, (void*)0);
    f->glBindVertexArray(0);

    queryTime(4);

    skyboxProgram->bind();

    skyboxProgram->setUniformValue("skyboxTexture", 7);
    f->glActiveTexture(GL_TEXTURE7);
    f->glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTex);

    skyboxProgram->setUniformValue("modelMat", QMatrix4x4());
    skyboxProgram->setUniformValue("viewMat", scene->getCamera()->rotation());
    skyboxProgram->setUniformValue("projMat", scene->getCamera()->projectionMatrix());
    f->glEnable(GL_DEPTH_TEST);
    f->glDepthFunc(GL_LEQUAL);
    f->glBindVertexArray(skyboxVAO);
    f->glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    f->glBindVertexArray(0);

    queryTime(5);

    logTimes();

    useQueryB = !useQueryB;
    frameCounter++;
}
