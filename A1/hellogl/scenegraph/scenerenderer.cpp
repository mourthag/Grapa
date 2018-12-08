#include "scenerenderer.h"

static const GLfloat quad_UV[] = {
    0.0, 0.0,
    1.0, 0.0,
    0.0, 1.0,
    1.0, 1.0
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
    performanceLogger->addSeries(NumQueries-1);
    performanceLogger->setName(0, "Compute LOD");
    performanceLogger->setName(1, "Terrain");
    performanceLogger->setName(2, "Tree Geometry");
    performanceLogger->setName(3, "Tree Impostor");
    performanceLogger->setName(4, "Skybox");

    queryObjectsA = std::vector<GLuint>(NumQueries);
    queryObjectsB = std::vector<GLuint>(NumQueries);
    queryResultsA = std::vector<GLuint64>(NumQueries);
    queryResultsB = std::vector<GLuint64>(NumQueries);
}

void SceneRenderer::setUpFBOTextures()
{
    OpenGLFunctions *f = OpenGLFunctions::instance();

    f->glGenTextures(1, &ntcTex);
    f->glBindTexture(GL_TEXTURE_2D, ntcTex);
    f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    f->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, scr_size, scr_size, 0, GL_RGBA, GL_FLOAT, NULL);
    f->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ntcTex, 0);

    f->glGenTextures(1, &matTex);
    f->glBindTexture(GL_TEXTURE_2D, matTex);
    f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    f->glTexImage2D(GL_TEXTURE_2D, 0, GL_R8UI, scr_size, scr_size, 0, GL_RED_INTEGER, GL_UNSIGNED_BYTE, NULL);

    f->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, matTex, 0);

    f->glGenTextures(1, &depthTex);
    f->glBindTexture(GL_TEXTURE_2D, depthTex);
    f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);

    f->glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, scr_size, scr_size, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    f->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTex, 0);
    f->glBindTexture(GL_TEXTURE_2D, 0);
}


void SceneRenderer::setUpQuad()
{
    OpenGLFunctions *f = OpenGLFunctions::instance();

    f->glGenVertexArrays(1, &quadVAO);
    f->glBindVertexArray(quadVAO);

    f->glGenBuffers(1, &quadVBO);
    f->glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    f->glBufferData(GL_ARRAY_BUFFER,  8 * sizeof(GLfloat), &quad_UV, GL_STATIC_DRAW);
    deferredLightingPassProgram->setAttributeBuffer("UV", GL_FLOAT, 0, 2);

    f->glGenBuffers(1, &quadIBO);
    f->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quadIBO);
    f->glBufferData(GL_ELEMENT_ARRAY_BUFFER, 4 * sizeof(GLuint), &quad_Ind, GL_STATIC_DRAW);
    f->glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
    f->glEnableVertexAttribArray(0);

    f->glBindVertexArray(0);
    f->glBindBuffer(GL_ARRAY_BUFFER, 0);
    f->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void SceneRenderer::init() {

    OpenGLFunctions *f = OpenGLFunctions::instance();

    f->glClearColor(1,1,1,1);

    f->glGenQueries(NumQueries, queryObjectsA.data());
    f->glGenQueries(NumQueries, queryObjectsB.data());

    phongProgram = new QOpenGLShaderProgram();
    phongProgram->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shader/default/phongvertshader.vert");
    phongProgram->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shader/default/phongfragshader.frag");
    phongProgram->link();

    deferredGeomPassProgram = new QOpenGLShaderProgram();
    deferredGeomPassProgram->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shader/deferred/geometrypassvertshader.vert");
    deferredGeomPassProgram->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shader/deferred/geometrypassfragshader.frag");
    deferredGeomPassProgram->link();

    deferredLightingPassProgram = new QOpenGLShaderProgram();
    deferredLightingPassProgram->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shader/deferred/lightingpassvertshader.vert");
    deferredLightingPassProgram->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shader/deferred/lightingpassfragshader.frag");
    deferredLightingPassProgram->link();

    f->glGenFramebuffers(1, &fbo);
    f->glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    setUpFBOTextures();

    unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
    f->glDrawBuffers(2,  attachments);

    GLenum fbo_complete = f->glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if(fbo_complete != GL_FRAMEBUFFER_COMPLETE)
        qDebug() << "Framebuffer incomplete!";

    f->glBindFramebuffer(GL_FRAMEBUFFER, 0);
    f->glBindTexture(GL_TEXTURE_2D, 0);

    setUpQuad();

}

void SceneRenderer::updateFramebuffeSize(int width, int height) {

    OpenGLFunctions *f = OpenGLFunctions::instance();

    f->glBindTexture(GL_TEXTURE_2D, ntcTex);
    f->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_FLOAT, NULL);

    f->glBindTexture(GL_TEXTURE_2D, matTex);
    f->glTexImage2D(GL_TEXTURE_2D, 0, GL_R8UI, width, height, 0, GL_RED_INTEGER, GL_UNSIGNED_BYTE, NULL);

    f->glBindTexture(GL_TEXTURE_2D, depthTex);
    f->glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    f->glBindTexture(GL_TEXTURE_2D, 0);

}

Light* SceneRenderer::getLight() {
    return &light;
}

void SceneRenderer::queryTime(int index)
{
    OpenGLFunctions *f = OpenGLFunctions::instance();

    if(useQueryB)
        f->glQueryCounter(queryObjectsB[index], GL_TIMESTAMP);
    else
        f->glQueryCounter(queryObjectsA[index], GL_TIMESTAMP);
}

void SceneRenderer::logTimes()
{
    OpenGLFunctions *f = OpenGLFunctions::instance();

    for (auto i = 0 ; i < NumQueries ; ++i ) {
        if(useQueryB) {
            f->glGetQueryObjectui64v(queryObjectsA[i], GL_QUERY_RESULT , queryResultsA.data() + i);
        }
        else if(frameCounter > 0){
            f->glGetQueryObjectui64v(queryObjectsB[i], GL_QUERY_RESULT , queryResultsB.data() + i);
        }
        if(i != 0 & frameCounter % 33 == 0) {
            performanceLogger->addData(frameCounter, queryResultsA[i] - queryResultsA[i-1], i-1);
        }
    }
}

void SceneRenderer::drawScene(Scene *scene) {
    OpenGLFunctions *f = OpenGLFunctions::instance();

    QOpenGLShaderProgram *activeProgram;
    UniformMode uniformMode;
    bool bufferUniformBlocks;

    GLint defaultFBO;
    f->glGetIntegerv(GL_FRAMEBUFFER_BINDING_EXT, &defaultFBO);

    if(mode == Phong) {
        activeProgram = phongProgram;
        uniformMode = PhongUniforms;
        bufferUniformBlocks = true;
    }
    else {

        activeProgram = deferredGeomPassProgram;
        uniformMode = GeometryPassUniforms;
        bufferUniformBlocks = false;

        f->glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        f->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    activeProgram->bind();
    setUpUniforms(activeProgram, uniformMode);

    queryTime(0);

    scene->drawScene(activeProgram, bufferUniformBlocks);

    queryTime(1);

    f->glBindFramebuffer(GL_FRAMEBUFFER, defaultFBO);

    if(mode != Phong) {
        deferredLightingPassProgram->bind();

        //Set up Textures;
        setUpUniforms(deferredLightingPassProgram, LightingPassUniforms);

        scene->setUpUniforms(deferredLightingPassProgram, true);

        f->glBindVertexArray(quadVAO);
        f->glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_INT, 0);
        f->glBindVertexArray(0);

    }

    queryTime(2);

    logTimes();

    useQueryB = !useQueryB;
    frameCounter++;

}

void SceneRenderer::setUpUniforms(QOpenGLShaderProgram *prog,UniformMode uniformMode) {
    OpenGLFunctions *f = OpenGLFunctions::instance();

    QMatrix4x4 idMat;
    prog->setUniformValue("modelMat", idMat);
    prog->setUniformValue("normalMat", idMat.normalMatrix());
    prog->setUniformValue("lightPos", light.lightPos);
    prog->setUniformValue("lightColor", light.lightCol);
    prog->setUniformValue("lightInt", light.lightInt);

    switch (uniformMode) {
    case PhongUniforms:

        break;
    case LightingPassUniforms:


        prog->setUniformValue("ntcTexture", 0);
        f->glActiveTexture(GL_TEXTURE0);
        f->glBindTexture(GL_TEXTURE_2D, ntcTex);

        prog->setUniformValue("materialTexture", 1);
        f->glActiveTexture(GL_TEXTURE1);
        f->glBindTexture(GL_TEXTURE_2D, matTex);

        prog->setUniformValue("depthTexture", 2);
        f->glActiveTexture(GL_TEXTURE2);
        f->glBindTexture(GL_TEXTURE_2D, depthTex);

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
