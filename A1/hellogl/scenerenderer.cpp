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

SceneRenderer::SceneRenderer()
{
    mode = RenderMode::Deferred;
}

void SceneRenderer::setUpFBOTextures()
{
    glGenTextures(1, &ntcTex);
    glBindTexture(GL_TEXTURE_2D, ntcTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, scr_size, scr_size, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ntcTex, 0);

    glGenTextures(1, &matTex);
    glBindTexture(GL_TEXTURE_2D, matTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, scr_size, scr_size, 0, GL_RED, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, matTex, 0);

    glGenTextures(1, &depthTex);
    glBindTexture(GL_TEXTURE_2D, depthTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, scr_size, scr_size, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTex, 0);
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

    phongProgram = new QOpenGLShaderProgram();
    phongProgram->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shader/phongvertshader.vert");
    phongProgram->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shader/phongfragshader.frag");
    phongProgram->link();

    deferredGeomPassProgram = new QOpenGLShaderProgram();
    deferredGeomPassProgram->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shader/geometrypassvertshader.vert");
    deferredGeomPassProgram->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shader/geometrypassfragshader.frag");
    deferredGeomPassProgram->link();

    deferredLightingPassProgram = new QOpenGLShaderProgram();
    deferredLightingPassProgram->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/shader/lightingpassvertshader.vert");
    deferredLightingPassProgram->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, ":/shader/lightingpassfragshader.frag");
    deferredLightingPassProgram->link();

    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    setUpFBOTextures();

    unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
    glDrawBuffers(3, attachments);

    GLenum fbo_complete = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if(fbo_complete != GL_FRAMEBUFFER_COMPLETE)
        qDebug() << "Framebuffer incomplete!";

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    setUpQuad();

}

void SceneRenderer::drawScene(Scene *scene) {

    QOpenGLShaderProgram *activeProgram;
    UniformMode uniformMode;

    GLint defaultFBO;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING_EXT, &defaultFBO);

    switch (mode) {
    case Phong:
        activeProgram = phongProgram;
        uniformMode = PhongUniforms;
        break;
    case Deferred:
        activeProgram = deferredGeomPassProgram;
        uniformMode = GeometryPassUniforms;

        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        break;
    default:

        break;
    }

    activeProgram->bind();
    setUpUniforms(activeProgram, uniformMode);
    scene->drawScene(activeProgram);

    glBindFramebuffer(GL_FRAMEBUFFER, defaultFBO);

    if(mode == Deferred) {
        deferredLightingPassProgram->bind();
        //Set up Textures;


        deferredLightingPassProgram->setUniformValue("ntcTexture", 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D_ARRAY, ntcTex);

        deferredLightingPassProgram->setUniformValue("materialTexture", 1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D_ARRAY, matTex);

        deferredLightingPassProgram->setUniformValue("depthTexture", 2);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D_ARRAY, depthTex);

        glBindVertexArray(quadVAO);
        glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

    }
}

void SceneRenderer::setUpUniforms(QOpenGLShaderProgram *prog,UniformMode uniformMode) {

    switch (uniformMode) {
    case PhongUniforms:

        break;
    case GeometryPassUniforms:

    default:
        break;
    }
}


void SceneRenderer::setRenderMode(RenderMode newMode) {
    mode = newMode;
}
