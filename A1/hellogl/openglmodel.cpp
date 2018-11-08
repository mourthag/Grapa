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

//void OpenGLModel::generateCube(int tesselation) {



//    int num_verts = (1 + tesselation)  * (1+tesselation) * 6;;

//    index_type = GL_UNSIGNED_INT;
//    index_offset = 0;
//    num_tris = 6 * tesselation * tesselation * 2;

//    std::vector<GLfloat> pos_data;
//    std::vector<GLuint> ind_data;
//    std::vector<GLfloat> normal_data;
//    std::vector<GLfloat> color_data;

//    //color for vertex(face)
//    QVector3D color;
//    //normal for vertex(face)
//    QVector3D normal;
//    //strides define how to iterate over the vertices  of a face
//    //this is important to define because you need to know this for indexing
//    QVector3D strideA;
//    QVector3D strideB;
//    //starting point of an iteration
//    QVector3D start;

//    //for each face define the variables from above
//    for(int i = 0; i <6; i++) {
//        switch (i){
//        //front
//        case 0:
//            color = QVector3D(1.0, 0.0, 0.0);
//            normal = QVector3D(0.0, 0.0, 1.0);
//            strideB = QVector3D(2.0, 0.0, 0.0) / tesselation;
//            strideA = QVector3D(0.0, 2.0, 0.0) / (float)tesselation;
//            break;
//        //behind
//        case 1:
//            color = QVector3D(1.0, 1.0, 0.0);
//            normal = QVector3D(0.0, 0.0, -1.0) ;
//            strideB = QVector3D(2.0, 0.0, 0.0) / tesselation;
//            strideA = QVector3D(0.0, 2.0, 0.0) / tesselation;
//            break;
//        //up
//        case 2:
//            color = QVector3D(0.0, 1.0, 0.0);
//            normal = QVector3D(0.0, 1.0, 0.0);
//            strideA = QVector3D(2.0, 0.0, 0.0) / tesselation;
//            strideB = QVector3D(0.0, 0.0, 2.0) / tesselation;
//            break;
//        //down
//        case 3:
//            color = QVector3D(0.0, 1.0, 1.0);
//            normal = QVector3D(0.0, -1.0, 0.0);
//            strideB = QVector3D(2.0, 0.0, 0.0) / tesselation;
//            strideA = QVector3D(0.0, 0.0, 2.0) / tesselation;
//            break;
//        //right
//        case 4:
//            color = QVector3D(0.0, 0.0, 1.0);
//            normal = QVector3D(1.0, 0.0, 0.0);
//            strideB = QVector3D(0.0, 2.0, 0.0) / tesselation;
//            strideA = QVector3D(0.0, 0.0, 2.0) / tesselation;
//            break;
//        //left
//        case 5:
//            color = QVector3D(1.0, 0.0, 1.0);
//            normal = QVector3D(-1.0, 0.0, 0.0);
//            strideA = QVector3D(0.0, 2.0, 0.0) / tesselation;
//            strideB = QVector3D(0.0, 0.0, 2.0) / tesselation;
//            break;
//        }
//        //set start to (relative) bottom left of the face
//        start = normal - tesselation * 0.5 * (strideA + strideB);

//        //vertex index offset from faces
//        int offset = i * (tesselation+1) *(tesselation+1);

//        //iterate over vertices of the face
//        for(int j = 0; j < tesselation+1; j++) {
//            for(int k = 0; k < tesselation+1; k++) {
//                //calculate position
//                QVector3D vert = start + j * strideA + k * strideB;
//                //add vertex position
//                pos_data.push_back(vert.x());
//                pos_data.push_back(vert.y());
//                pos_data.push_back(vert.z());

//                //add color
//                color_data.push_back(color.x());
//                color_data.push_back(color.y());
//                color_data.push_back(color.z());

//                //add normal
//                normal_data.push_back(normal.x());
//                normal_data.push_back(normal.y());
//                normal_data.push_back(normal.z());

//                //calculate index of current triangle
//                int index = offset + j * (tesselation+1) +k;
//                //create two triangles per vertex that is not in the last row or in the last column
//                if(j != tesselation && k != tesselation) {

//                    //bottom left triangle
//                    ind_data.push_back(index);
//                    ind_data.push_back(index + 1);
//                    ind_data.push_back(index + (tesselation+1));

//                    //top right triangle
//                    ind_data.push_back(index + 1);
//                    ind_data.push_back(index + 1 + (tesselation+1));
//                    ind_data.push_back(index + (tesselation+1));
//                }
//            }
//        }
//    }

//    glBindVertexArray(vao);

//    glBindBuffer(GL_ARRAY_BUFFER, vbo);
//    glBufferData(GL_ARRAY_BUFFER, 3 * num_verts * sizeof(GLfloat), &pos_data[0], GL_STATIC_DRAW);
//    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
//    glEnableVertexAttribArray(0);

//    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
//    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3 * num_tris * sizeof(GLuint), &ind_data[0], GL_STATIC_DRAW);

//    glBindBuffer(GL_ARRAY_BUFFER, nbo);
//    glBufferData(GL_ARRAY_BUFFER, 3 * num_verts * sizeof(GLfloat), &normal_data[0], GL_STATIC_DRAW);

//    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
//    glEnableVertexAttribArray(1);

//    glBindBuffer(GL_ARRAY_BUFFER, cbo);
//    glBufferData(GL_ARRAY_BUFFER, 3 * num_verts * sizeof(GLfloat), &color_data[0], GL_STATIC_DRAW);

//    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
//    glEnableVertexAttribArray(2);

//    glBindVertexArray(0);
//    glBindBuffer(GL_ARRAY_BUFFER, 0);
//    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
//}

void OpenGLModel::loadGLTF(QOpenGLShaderProgram *prog, tinygltf::Model *gltf_model, int mesh, int primIndex) {


    glBindVertexArray(vao);

    GLint attributeIndex = prog->attributeLocation("pos");
    loadGLTFAttribute("POSITION", gltf_model, mesh, primIndex, vbo, attributeIndex);
    attributeIndex = prog->attributeLocation("vertnormal");
    loadGLTFAttribute("NORMAL", gltf_model, mesh, primIndex, nbo, attributeIndex);
    attributeIndex = prog->attributeLocation("UV");
    loadGLTFAttribute("TEXCOORD_0", gltf_model, mesh, primIndex, tcbo, attributeIndex);

    loadGLTFIndices(gltf_model, mesh, primIndex);

    materialIndex = gltf_model->meshes[mesh].primitives[primIndex].material;

//    glBindVertexArray(0);
//    glBindBuffer(GL_ARRAY_BUFFER, 0);
//    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

}

void OpenGLModel::loadGLTFAttribute(std::string name, tinygltf::Model *model, int mesh, int primitive, GLuint glBufferIndex, GLuint glAttributeIndex) {

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

void OpenGLModel::drawModel() {

    glBindVertexArray(vao);
    qDebug() << vao << " at: " << &vao;
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glDrawElements(GL_TRIANGLES, 3 * num_tris, index_type, (void*)index_offset);
    glBindVertexArray(0);
}

void OpenGLModel::setUpDrawing(QOpenGLShaderProgram *program, QMatrix4x4 *viewMat) {    //for shading calculations in viewspace

    program->setUniformValue("m", model_mat);
    program->setUniformValue("normalMat", (*viewMat * model_mat).normalMatrix());
    program->setUniformValue("materialIndex", materialIndex);

}
