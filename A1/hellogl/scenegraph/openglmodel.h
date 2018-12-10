#ifndef OPENGLMODEL_H
#define OPENGLMODEL_H

#include <QOpenGLShaderProgram>
#define TINYGLTF_NO_STB_IMAGE_WRITE

#include "tiny_gltf.h"
#include "util/openglfunctions.h"

class OpenGLModel
{


public:
    OpenGLModel();
    ~OpenGLModel();

    void loadGLTF(tinygltf::Model *gltf_model, int mesh, int primIndex, bool additive);
    //void generateCube(int tesselation);

    void clear();
    void drawModel(QOpenGLShaderProgram *program);
    QVector3D getBoundingBoxMinimum() {
        return bbMin;
    }
    QVector3D getBoundingBoxMaximum() {
        return bbMax;
    }

    GLuint vao;

    int materialIndex;

    std::vector<QVector3D> vertices;
    GLenum index_type;
    size_t index_offset;
    int num_verts;

private:
    void loadGLTFAttribute(std::string name, tinygltf::Model *model, int mesh, int primitive, GLuint glBufferIndex, GLint glAttributeIndex, bool additive);
    void loadGLTFIndices(tinygltf::Model *model, int mesh, int primitive, bool additive);
    void convertBuffer(int size, int offset, int stride, int length, std::vector<unsigned char> *data, std::vector<GLfloat> *convertedData);
    void adjustVBOSize(int numberElements);
    void loadVertices(int offset, int length, std::vector<unsigned char> *rawData);

    GLuint ebo;
    GLuint vbo;
    GLuint nbo;
    GLuint tcbo;

    QVector3D bbMin;
    QVector3D bbMax;


};

#endif // OPENGLMODEL_H
