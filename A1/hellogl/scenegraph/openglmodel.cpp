#include "openglmodel.h"

OpenGLModel::OpenGLModel()
{
    initializeOpenGLFunctions();
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &ebo);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &nbo);
    glGenBuffers(1, &tcbo);

}

OpenGLModel::~OpenGLModel() {
}

void OpenGLModel::clear() {
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &ebo);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &nbo);
    glDeleteBuffers(1, &tcbo);
}
void OpenGLModel::loadGLTF(tinygltf::Model *gltf_model, int mesh, int primIndex, bool additive) {

    glBindVertexArray(vao);

    loadGLTFAttribute("POSITION", gltf_model, mesh, primIndex, vbo, 0, additive);
    loadGLTFAttribute("NORMAL", gltf_model, mesh, primIndex, nbo, 1, additive);
    loadGLTFAttribute("TEXCOORD_0", gltf_model, mesh, primIndex, tcbo, 2, additive);

    loadGLTFIndices(gltf_model, mesh, primIndex, additive);

    materialIndex = gltf_model->meshes[mesh].primitives[primIndex].material;

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

}


void OpenGLModel::loadGLTFAttribute(std::string name, tinygltf::Model *model, int mesh, int primitive, GLuint glBufferIndex, GLint glAttributeIndex, bool additive) {

    tinygltf::Primitive prim = model->meshes[mesh].primitives[primitive];

    if(prim.attributes.find(name) == prim.attributes.end())
        return;

    int attributeID = model->meshes[mesh].primitives[primitive].attributes[name];

    tinygltf::Accessor accessor = model->accessors[attributeID];
    tinygltf::BufferView bufferView = model->bufferViews[accessor.bufferView];
    tinygltf::Buffer buffer = model->buffers[bufferView.buffer];

    int size = accessor.type;

    glBindBuffer(bufferView.target, glBufferIndex);
    if(additive) {
        GLint bufferSize;
        glGetBufferParameteriv(bufferView.target, GL_BUFFER_SIZE, &bufferSize);

        if(bufferSize > 0) {
            std::vector<GLfloat> oldData(bufferSize/sizeof(GLfloat));

            glGetBufferSubData(bufferView.target, 0, bufferSize, &oldData.at(0));

            glBufferData(bufferView.target, bufferSize + bufferView.byteLength, NULL, GL_STATIC_DRAW);
            glBufferSubData(bufferView.target, 0, bufferSize, &oldData.at(0));
            glBufferSubData(bufferView.target, bufferSize, bufferView.byteLength, &buffer.data[bufferView.byteOffset]);
        }
        else {
            glBufferData(bufferView.target, bufferView.byteLength, &buffer.data[bufferView.byteOffset], GL_STATIC_DRAW);

        }

    }
    else{
        glBufferData(bufferView.target, bufferView.byteLength, &buffer.data[bufferView.byteOffset], GL_STATIC_DRAW);
    }

    glVertexAttribPointer(glAttributeIndex, size, accessor.componentType, GL_FALSE, bufferView.byteStride, (void*)accessor.byteOffset);
    glEnableVertexAttribArray(glAttributeIndex);
}

void OpenGLModel::loadGLTFIndices(tinygltf::Model *model, int mesh, int primitive, bool additive) {
    int attributeID = model->meshes[mesh].primitives[primitive].indices;

    tinygltf::Accessor accessor = model->accessors[attributeID];
    tinygltf::BufferView bufferView = model->bufferViews[accessor.bufferView];
    tinygltf::Buffer buffer = model->buffers[bufferView.buffer];

    glBindBuffer(bufferView.target, ebo);    if(additive) {
        GLint bufferSize;
        glGetBufferParameteriv(bufferView.target, GL_BUFFER_SIZE, &bufferSize);
        if(bufferSize > 0) {
            std::vector<GLfloat> oldData(bufferSize/sizeof(GLfloat));

            glGetBufferSubData(bufferView.target, 0, bufferSize, &oldData.at(0));

            glBufferData(bufferView.target, bufferSize + bufferView.byteLength, NULL, GL_STATIC_DRAW);
            glBufferSubData(bufferView.target, 0, bufferSize, &oldData.at(0));
            glBufferSubData(bufferView.target, bufferSize, bufferView.byteLength, &buffer.data[bufferView.byteOffset]);
            index_offset += accessor.byteOffset;
            num_verts += accessor.count;
        }
        else {
            glBufferData(bufferView.target, bufferView.byteLength, &buffer.data[bufferView.byteOffset], GL_STATIC_DRAW);
            index_type = accessor.componentType;
            index_offset = accessor.byteOffset;
            num_verts = accessor.count;
        }

    }
    else{
        glBufferData(bufferView.target, bufferView.byteLength, &buffer.data[bufferView.byteOffset], GL_STATIC_DRAW);
        index_type = accessor.componentType;
        index_offset = accessor.byteOffset;
        num_verts = accessor.count;
    }


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
    glDrawElements(GL_TRIANGLES, 3 * num_verts, index_type, (void*)index_offset);
    glBindVertexArray(0);
}

