#ifndef OPENGLMODEL_H
#define OPENGLMODEL_H

#include <QOpenGLFunctions_4_0_Core>
#include <QOpenGLShaderProgram>
#define TINYGLTF_NO_STB_IMAGE_WRITE

#include "tiny_gltf.h"

class OpenGLModel
{
public:
    OpenGLModel();
    static OpenGLModel FromGLTF(QOpenGLShaderProgram *prog, tinygltf::Model *gltf_model, int primIndex);
    static OpenGLModel GenerateCube(QOpenGLShaderProgram *prog, int tesselation);

    void clear();
    void drawModel(QOpenGLContext *context);
    void setUpDrawing(QOpenGLContext * context, QOpenGLShaderProgram *program, QMatrix4x4 *viewMat);

    QMatrix4x4 model_mat;
private:

    GLuint vao;
    GLuint vbo;
    GLuint ibo;
    GLuint cbo;
    GLuint nbo;

    GLenum index_type;
    size_t index_offset;
    int num_tris;

};

#endif // OPENGLMODEL_H
