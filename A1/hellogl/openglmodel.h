#ifndef OPENGLMODEL_H
#define OPENGLMODEL_H

#include <QOpenGLFunctions_4_0_Core>
#include <QOpenGLShaderProgram>
#define TINYGLTF_NO_STB_IMAGE_WRITE

#include "tiny_gltf.h"

class OpenGLModel
{
public:
    OpenGLModel(QOpenGLContext *context);
    static OpenGLModel FromGLTF(QOpenGLContext *context, QOpenGLShaderProgram *prog, tinygltf::Model *gltf_model, int primIndex);
    static OpenGLModel GenerateCube(QOpenGLContext *context, QOpenGLShaderProgram *prog, int tesselation);

    void drawModel(QOpenGLContext *context);


    GLuint vao;
    GLuint vbo;
    GLuint ibo;
    GLuint cbo;
    GLuint nbo;

    GLenum index_type;
    size_t index_offset;
    int num_tris;
private:
    QMatrix4x4 model_mat;

};

#endif // OPENGLMODEL_H
