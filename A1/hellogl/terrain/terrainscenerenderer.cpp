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
    numImpostorImages = 1;
    terrainDrawEnabled = true;
    tesselation = 1;
    treeDrawEnabled = true;
    frustumCullingEnabled = true;

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

void TerrainSceneRenderer::setNumImpostors(int val) {
    numImpostorImages = val;
    OpenGLFunctions *f = OpenGLFunctions::instance();
    f->glBindTexture(GL_TEXTURE_2D_ARRAY, impostorTex);
    f->glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA8,
                    impostorTexSize, impostorTexSize, numImpostorImages,
                    0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
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
                    impostorTexSize, impostorTexSize, numImpostorImages,
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

void TerrainSceneRenderer::createImpostorTex(TerrainScene *scene) {

    Tree *tree = &scene->tree;
    OpenGLFunctions *f = OpenGLFunctions::instance();

    f->glBindFramebuffer(GL_FRAMEBUFFER, impostorFBO);
    f->glBindTexture(GL_TEXTURE_2D_ARRAY, impostorTex);
    f->glViewport(0,0,impostorTexSize,impostorTexSize);
    phongProgram->bind();
    scene->setUpUniforms(phongProgram, true);
    QMatrix4x4 projMatrix;
    QVector3D min = 1.2 * tree->boundingBoxMin;
    QVector3D max = 1.2 * tree->boundingBoxMax;
    QVector3D center = min * 0.5 + max * 0.5;
    QMatrix4x4 idMat;
    QMatrix4x4 rotMat;
    rotMat.rotate(-90, 1, 0, 0);

    QMatrix4x4 transMat;

    transMat.translate(center);

    min = rotMat.map(min);
    max = rotMat.map(max);

    projMatrix.ortho(min.x(), max.x(), min.y(), max.y(), min.z(), max.z());
    phongProgram->setUniformValue("projMat", projMatrix);
    phongProgram->setUniformValue("modelMat", rotMat);
    phongProgram->setUniformValue("normalMat", rotMat.normalMatrix());
    phongProgram->setUniformValue("viewMat", idMat);
    phongProgram->setUniformValue("lightPos", QVector3D(0,0,-1));
    phongProgram->setUniformValue("lightColor", QVector3D(1,1,1));
    phongProgram->setUniformValue("lightInt", (GLfloat) 1.0);
    for(int i = 0; i < numImpostorImages; i++) {
        f->glViewport(0,0,impostorTexSize,impostorTexSize);
        f->glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, impostorTex, 0, i);
        f->glClearColor(1,1,1,0);
        f->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        for(int j =0; j< tree->meshes.size(); j++) {

            f->glBindVertexArray(tree->meshes[j]->vao);
            phongProgram->setUniformValue("materialIndex", tree->meshes[j]->materialIndex);
            f->glDrawElements(GL_TRIANGLES, tree->meshes[j]->num_verts, tree->meshes[j]->index_type, (void*) tree->meshes[j]->index_offset);
            f->glBindVertexArray(0);
        }
        //f->glFinish();
        rotMat.rotate(360.0/(float)(numImpostorImages+1), 0, 0, 1);
        phongProgram->setUniformValue("modelMat", rotMat);
        phongProgram->setUniformValue("normalMat", rotMat.normalMatrix());
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

void TerrainSceneRenderer::executeLODCompute(TerrainScene *scene, OpenGLFunctions *f)
{
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
    scene->terrain.setHeightMapUniform(treeDataProgram);

    scene->setUpCameraUniforms(treeDataProgram);
    GLint workGroupSize[3];
    f->glGetProgramiv(treeDataProgram->programId(), GL_COMPUTE_WORK_GROUP_SIZE, workGroupSize);
    GLint numInvocations = scene->forrest.getNumTrees()/workGroupSize[0] + 1;
    f->glDispatchCompute(numInvocations, 1, 1);
    f->glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
}

void TerrainSceneRenderer::drawTerrain(TerrainScene *scene)
{
    terrainProgram->bind();
    terrainProgram->setUniformValue("tessLevel", tesselation);
    setUpUniforms(terrainProgram, PhongUniforms);
    scene->drawTerrain(terrainProgram);
}

void TerrainSceneRenderer::drawGeometryTrees(OpenGLFunctions *f, TerrainScene *scene)
{
    treeProgram->bind();
    setUpUniforms(treeProgram, PhongUniforms);
    QMatrix4x4 test;
    test.rotate(-90, 1, 0, 0);
    treeProgram->setUniformValue("modelMat", test);

    f->glBindBuffer(GL_DRAW_INDIRECT_BUFFER, drawCommandBuffer);

    scene->setUpUniforms(treeProgram, true);
    scene->setUpCameraUniforms(treeProgram);
    scene->terrain.setHeightMapUniform(treeProgram);

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

void TerrainSceneRenderer::drawImpostorTrees(OpenGLFunctions *f, TerrainScene *scene)
{
    treeImpostorProgram->bind();
    scene->setUpUniforms(treeImpostorProgram, false);
    scene->setUpCameraUniforms(treeImpostorProgram);
    scene->terrain.setHeightMapUniform(treeImpostorProgram);

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

void TerrainSceneRenderer::drawSkybox(TerrainScene *scene, OpenGLFunctions *f)
{
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
}

void TerrainSceneRenderer::drawScene(TerrainScene *scene) {

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
