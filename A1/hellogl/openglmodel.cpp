#include "openglmodel.h"

OpenGLModel::OpenGLModel()
{
    initializeOpenGLFunctions();
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &ebo);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &nbo);
    glGenBuffers(1, &tcbo);

    qDebug() << "Allocated: " << vao << " at: " << &vao;
}

OpenGLModel::~OpenGLModel() {
}

void OpenGLModel::clear() {

    qDebug() << "Deleted: " << vao << " at: " << &vao;
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &ebo);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &nbo);
    glDeleteBuffers(1, &tcbo);
}
void OpenGLModel::loadGLTF(tinygltf::Model *gltf_model, int mesh, int primIndex) {

    glBindVertexArray(vao);

    loadGLTFAttribute("POSITION", gltf_model, mesh, primIndex, vbo, 0);
    loadGLTFAttribute("NORMAL", gltf_model, mesh, primIndex, nbo, 1);
    loadGLTFAttribute("TEXCOORD_0", gltf_model, mesh, primIndex, tcbo, 2);

    loadGLTFIndices(gltf_model, mesh, primIndex);

    materialIndex = gltf_model->meshes[mesh].primitives[primIndex].material;

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

}

void OpenGLModel::loadGLTFAttribute(std::string name, tinygltf::Model *model, int mesh, int primitive, GLuint glBufferIndex, GLint glAttributeIndex) {

    int attributeID = model->meshes[mesh].primitives[primitive].attributes[name];

    tinygltf::Accessor accessor = model->accessors[attributeID];
    tinygltf::BufferView bufferView = model->bufferViews[accessor.bufferView];
    tinygltf::Buffer buffer = model->buffers[bufferView.buffer];

    int size = accessor.type;

    glBindBuffer(bufferView.target, glBufferIndex);
    glBufferData(bufferView.target, bufferView.byteLength, &buffer.data[bufferView.byteOffset], GL_STATIC_DRAW);

    glVertexAttribPointer(glAttributeIndex, size, accessor.componentType, GL_FALSE, bufferView.byteStride, (void*)accessor.byteOffset);
    glEnableVertexAttribArray(glAttributeIndex);
}

void OpenGLModel::loadGLTFIndices(tinygltf::Model *model, int mesh, int primitive) {
    int attributeID = model->meshes[mesh].primitives[primitive].indices;

    tinygltf::Accessor accessor = model->accessors[attributeID];
    tinygltf::BufferView bufferView = model->bufferViews[accessor.bufferView];
    tinygltf::Buffer buffer = model->buffers[bufferView.buffer];

    glBindBuffer(bufferView.target, ebo);
    glBufferData(bufferView.target, bufferView.byteLength, &buffer.data[bufferView.byteOffset], GL_STATIC_DRAW);

    index_type = accessor.componentType;
    index_offset = accessor.byteOffset;
    num_tris = accessor.count / 3;
}

void OpenGLModel::convertBuffer(const int size, int offset, int stride, int length, std::vector<unsigned char> *data, std::vector<GLfloat> *convertedData) {

    for(int i=offset; i < length; i += stride) {
        //map memory from the binary buffer to an array of GLfloats
        GLfloat* value = new GLfloat[size];
        memcpy(&value[0], &(*data)[i], size * sizeof(GLfloat));

        //push values of array to the new buffer
        for(int j = 0; j < size; j++) {
            convertedData->push_back(value[j]);
        }
    }

}

void OpenGLModel::drawModel(QOpenGLShaderProgram *program) {

    program->setUniformValue("materialIndex", materialIndex);
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, 3 * num_tris, index_type, (void*)index_offset);
    glBindVertexArray(0);
}

