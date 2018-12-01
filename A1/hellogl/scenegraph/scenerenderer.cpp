#include "scenerenderer.h"

static const GLfloat quad_UV[] = {
    0.0, 0.0,
    1.0, 0.0,
    0.0, 1.0,
    1.0, 1.1
};

static const GLuint quad_Ind[] = {
    0, 1, 2, 3
};

void SceneRenderer::setTesselation(int t) {
    tesselation = t;
}

SceneRenderer::SceneRenderer()
{
    mode = RenderMode::Deferred;

    frameCounter = 0;

    useQueryB = false;
    performanceLogger = new PerformanceChart();

    queryObjectsA = std::vector<GLuint>(NumQueries);
    queryObjectsB = std::vector<GLuint>(NumQueries);
    queryResultsA = std::vector<GLuint64>(NumQueries);
    queryResultsB = std::vector<GLuint64>(NumQueries);
}

void SceneRenderer::setUpFBOTextures()
{
    glGenTextures(1, &ntcTex);
    glBindTexture(GL_TEXTURE_2D, ntcTex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, scr_size, scr_size, 0, GL_RGBA, GL_FLOAT, NULL);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ntcTex, 0);

    glGenTextures(1, &matTex);
    glBindTexture(GL_TEXTURE_2D, matTex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R8UI, scr_size, scr_size, 0, GL_RED_INTEGER, GL_UNSIGNED_BYTE, NULL);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, matTex, 0);

    glGenTextures(1, &depthTex);
    glBindTexture(GL_TEXTURE_2D, depthTex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, scr_size, scr_size, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTex, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
}


void SceneRenderer::setUpQuad()
{
    glGenVertexArrays(1, &quadVAO);
    glBindVertexArray(quadVAO);

    glGenBuffers(1, &quadVBO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER,  8 * sizeof(GLfloat), &quad_UV, GL_STATIC_DRAW);
    deferredLightingPassProgram->setAttributeBuffer("UV", GL_FLOAT, 0, 2);

    glGenBuffers(1, &quadIBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quadIBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 4 * sizeof(GLuint), &quad_Ind, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void SceneRenderer::initGL() {
    initializeOpenGLFunctions();
    glClearColor(1,1,1,1);

    glGenQueries(NumQueries, queryObjectsA.data());
    glGenQueries(NumQueries, queryObjectsB.data());

    phongProgram = new QOpenGLShaderProgram();
    phongProgram->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shader/phongvertshader.vert");
    phongProgram->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shader/phongfragshader.frag");
    phongProgram->link();

    deferredGeomPassProgram = new QOpenGLShaderProgram();
    deferredGeomPassProgram->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shader/geometrypassvertshader.vert");
    deferredGeomPassProgram->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shader/geometrypassfragshader.frag");
    deferredGeomPassProgram->link();

    deferredLightingPassProgram = new QOpenGLShaderProgram();
    deferredLightingPassProgram->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shader/lightingpassvertshader.vert");
    deferredLightingPassProgram->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shader/lightingpassfragshader.frag");
    deferredLightingPassProgram->link();

    terrainProgram = new QOpenGLShaderProgram();
    terrainProgram->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shader/terrainvertshader.vert");
    terrainProgram->addShaderFromSourceFile(QOpenGLShader::TessellationControl, ":/shader/terraintesselationcontrolshader.tcs");
    terrainProgram->addShaderFromSourceFile(QOpenGLShader::TessellationEvaluation, ":/shader/terraintesselationevalshader.tes");
    terrainProgram->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shader/terrainfragshader.frag");
    terrainProgram->link();

    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    setUpFBOTextures();

    unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
    glDrawBuffers(2,  attachments);

    GLenum fbo_complete = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if(fbo_complete != GL_FRAMEBUFFER_COMPLETE)
        qDebug() << "Framebuffer incomplete!";

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    setUpQuad();

}

void SceneRenderer::updateFramebuffeSize(int width, int height) {

    glBindTexture(GL_TEXTURE_2D, ntcTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_FLOAT, NULL);

    glBindTexture(GL_TEXTURE_2D, matTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R8UI, width, height, 0, GL_RED_INTEGER, GL_UNSIGNED_BYTE, NULL);

    glBindTexture(GL_TEXTURE_2D, depthTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glBindTexture(GL_TEXTURE_2D, 0);

}

void SceneRenderer::queryTime(int index)
{
    if(useQueryB)
        glQueryCounter(queryObjectsB[index], GL_TIMESTAMP);
    else
        glQueryCounter(queryObjectsA[index], GL_TIMESTAMP);
}

void SceneRenderer::logTimes()
{
    for (auto i = 0 ; i < NumQueries ; ++i ) {
        if(useQueryB) {
            glGetQueryObjectui64v(queryObjectsA[i], GL_QUERY_RESULT , queryResultsA.data() + i);
        }
        else if(frameCounter > 0){
            glGetQueryObjectui64v(queryObjectsB[i], GL_QUERY_RESULT , queryResultsB.data() + i);
        }
    }
    if(frameCounter % 33 == 0) {

        if(useQueryB)
            performanceLogger->addData(frameCounter, queryResultsA[1] - queryResultsA[0], queryResultsA[2] - queryResultsA[1]);
        else
            performanceLogger->addData(frameCounter, queryResultsB[1] - queryResultsB[0], queryResultsB[2] - queryResultsB[1] );

    }
}

void SceneRenderer::drawScene(Scene *scene) {

    QOpenGLShaderProgram *activeProgram;
    UniformMode uniformMode;
    bool bufferUniformBLocks;

    GLint defaultFBO;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING_EXT, &defaultFBO);

    if(mode == Phong) {
        activeProgram = phongProgram;
        uniformMode = PhongUniforms;
        bufferUniformBLocks = true;
    }
    else {

        activeProgram = deferredGeomPassProgram;
        uniformMode = GeometryPassUniforms;
        bufferUniformBLocks = false;

        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    activeProgram->bind();
    setUpUniforms(activeProgram, uniformMode);

    queryTime(0);

    scene->drawScene(activeProgram, bufferUniformBLocks);
    terrainProgram->bind();
    terrainProgram->setUniformValue("tessLevel", tesselation);
    scene->drawTerrain(terrainProgram);

    queryTime(1);

    glBindFramebuffer(GL_FRAMEBUFFER, defaultFBO);

    if(mode != Phong) {
        deferredLightingPassProgram->bind();

        //Set up Textures;
        setUpUniforms(deferredLightingPassProgram, LightingPassUniforms);

        scene->setUpUniforms(deferredLightingPassProgram, true);

        glBindVertexArray(quadVAO);
        glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

    }

    queryTime(2);

    logTimes();

    useQueryB = !useQueryB;
    frameCounter++;

}

void SceneRenderer::setUpUniforms(QOpenGLShaderProgram *prog,UniformMode uniformMode) {

    switch (uniformMode) {
    case PhongUniforms:

        break;
    case LightingPassUniforms:


        prog->setUniformValue("ntcTexture", 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, ntcTex);

        prog->setUniformValue("materialTexture", 1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, matTex);

        prog->setUniformValue("depthTexture", 2);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, depthTex);

        prog->setUniformValue("renderMode", int(mode));
        break;


    default:
        break;
    }
}

PerformanceChart* SceneRenderer::getLogger() {
    return performanceLogger;
}

void SceneRenderer::setRenderMode(RenderMode newMode) {
    mode = newMode;
}
