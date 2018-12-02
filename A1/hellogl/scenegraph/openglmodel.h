#ifndef OPENGLMODEL_H
#define OPENGLMODEL_H

#include <QOpenGLFunctions_4_0_Core>
#include <QOpenGLShaderProgram>
#define TINYGLTF_NO_STB_IMAGE_WRITE

#include "tiny_gltf.h"

class OpenGLModel : QOpenGLFunctions_4_0_Core
{


public:
    OpenGLModel();
    ~OpenGLModel();

    void loadGLTF(tinygltf::Model *gltf_model, int mesh, int primIndex);
    //void generateCube(int tesselation);

    void clear();
    void drawModel(QOpenGLShaderProgram *program);

    GLuint vao;

    int materialIndex;

    GLenum index_type;
    size_t index_offset;
    int num_tris;

private:
    void loadGLTFAttribute(std::string name, tinygltf::Model *model, int mesh, int primitive, GLuint glBufferIndex, GLint glAttributeIndex);
    void loadGLTFIndices(tinygltf::Model *model, int mesh, int primitive);
    void convertBuffer(int size, int offset, int stride, int length, std::vector<unsigned char> *data, std::vector<GLfloat> *convertedData);
    void adjustVBOSize(int numberElements);

    GLuint ebo;
    GLuint vbo;
    GLuint nbo;
    GLuint tcbo;


};

#endif // OPENGLMODEL_H
