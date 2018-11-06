#include "openglmodel.h"

OpenGLModel::OpenGLModel()
{

}

void OpenGLModel::clear() {

    QOpenGLFunctions_4_0_Core *f = new QOpenGLFunctions_4_0_Core;
    f->initializeOpenGLFunctions();

    f->glDisableVertexAttribArray(vao);
    f->glDeleteVertexArrays(1, &vao);
    f->glDeleteBuffers(1, &vbo);
    f->glDeleteBuffers(1, &ibo);
    f->glDeleteBuffers(1, &nbo);
    f->glDeleteBuffers(1, &cbo);
}

OpenGLModel OpenGLModel::GenerateCube(QOpenGLShaderProgram *prog, int tesselation) {
    OpenGLModel model = OpenGLModel();
    QOpenGLFunctions_4_0_Core *f = new QOpenGLFunctions_4_0_Core;
    f->initializeOpenGLFunctions();

    int num_verts = (1 + tesselation)  * (1+tesselation) * 6;;

    model.index_type = GL_UNSIGNED_INT;
    model.index_offset = 0;
    model.num_tris = 6 * tesselation * tesselation * 2;

    std::vector<GLfloat> vertex_position;
    std::vector<GLuint> vertex_index;
    std::vector<GLfloat> vertex_normal;
    std::vector<GLfloat> vertex_color;

    //color for vertex(face)
    QVector3D color;
    //normal for vertex(face)
    QVector3D normal;
    //strides define how to iterate over the vertices  of a face
    //this is important to define because you need to know this for indexing
    QVector3D strideA;
    QVector3D strideB;
    //starting point of an iteration
    QVector3D start;

    //for each face define the variables from above
    for(int i = 0; i <6; i++) {
        switch (i){
        //front
        case 0:
            color = QVector3D(1.0, 0.0, 0.0);
            normal = QVector3D(0.0, 0.0, 1.0);
            strideB = QVector3D(2.0, 0.0, 0.0) / tesselation;
            strideA = QVector3D(0.0, 2.0, 0.0) / (float)tesselation;
            break;
        //behind
        case 1:
            color = QVector3D(1.0, 1.0, 0.0);
            normal = QVector3D(0.0, 0.0, -1.0) ;
            strideB = QVector3D(2.0, 0.0, 0.0) / tesselation;
            strideA = QVector3D(0.0, 2.0, 0.0) / tesselation;
            break;
        //up
        case 2:
            color = QVector3D(0.0, 1.0, 0.0);
            normal = QVector3D(0.0, 1.0, 0.0);
            strideA = QVector3D(2.0, 0.0, 0.0) / tesselation;
            strideB = QVector3D(0.0, 0.0, 2.0) / tesselation;
            break;
        //down
        case 3:
            color = QVector3D(0.0, 1.0, 1.0);
            normal = QVector3D(0.0, -1.0, 0.0);
            strideB = QVector3D(2.0, 0.0, 0.0) / tesselation;
            strideA = QVector3D(0.0, 0.0, 2.0) / tesselation;
            break;
        //right
        case 4:
            color = QVector3D(0.0, 0.0, 1.0);
            normal = QVector3D(1.0, 0.0, 0.0);
            strideB = QVector3D(0.0, 2.0, 0.0) / tesselation;
            strideA = QVector3D(0.0, 0.0, 2.0) / tesselation;
            break;
        //left
        case 5:
            color = QVector3D(1.0, 0.0, 1.0);
            normal = QVector3D(-1.0, 0.0, 0.0);
            strideA = QVector3D(0.0, 2.0, 0.0) / tesselation;
            strideB = QVector3D(0.0, 0.0, 2.0) / tesselation;
            break;
        }
        //set start to (relative) bottom left of the face
        start = normal - tesselation * 0.5 * (strideA + strideB);

        //vertex index offset from faces
        int offset = i * (tesselation+1) *(tesselation+1);

        //iterate over vertices of the face
        for(int j = 0; j < tesselation+1; j++) {
            for(int k = 0; k < tesselation+1; k++) {
                //calculate position
                QVector3D vert = start + j * strideA + k * strideB;
                //add vertex position
                vertex_position.push_back(vert.x());
                vertex_position.push_back(vert.y());
                vertex_position.push_back(vert.z());

                //add color
                vertex_color.push_back(color.x());
                vertex_color.push_back(color.y());
                vertex_color.push_back(color.z());

                //add normal
                vertex_normal.push_back(normal.x());
                vertex_normal.push_back(normal.y());
                vertex_normal.push_back(normal.z());

                //calculate index of current triangle
                int index = offset + j * (tesselation+1) +k;
                //create two triangles per vertex that is not in the last row or in the last column
                if(j != tesselation && k != tesselation) {

                    //bottom left triangle
                    vertex_index.push_back(index);
                    vertex_index.push_back(index + 1);
                    vertex_index.push_back(index + (tesselation+1));

                    //top right triangle
                    vertex_index.push_back(index + 1);
                    vertex_index.push_back(index + 1 + (tesselation+1));
                    vertex_index.push_back(index + (tesselation+1));
                }
            }
        }
    }

    qDebug() << vertex_position[3 * vertex_index[0]]<< vertex_position[3 * vertex_index[0] + 1]<< vertex_position[3 * vertex_index[0] + 2];
    qDebug() << vertex_position[3 * vertex_index[1]]<< vertex_position[3 * vertex_index[1] + 1]<< vertex_position[3 * vertex_index[1] + 2];
    qDebug() << vertex_position[3 * vertex_index[2]]<< vertex_position[3 * vertex_index[2] + 1]<< vertex_position[3 * vertex_index[2] + 2];

    f->glGenVertexArrays(1, &model.vao);
    f->glBindVertexArray(model.vao);

    f->glGenBuffers(1, &model.vbo);
    f->glBindBuffer(GL_ARRAY_BUFFER, model.vbo);
    f->glBufferData(GL_ARRAY_BUFFER, 3 * num_verts * sizeof(GLfloat), &vertex_position[0], GL_STATIC_DRAW);

    prog->setAttributeBuffer("pos", GL_FLOAT, 0, 3);
    prog->enableAttributeArray("pos");

    f->glGenBuffers(1, &model.ibo);
    f->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model.ibo);
    f->glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3 * model.num_tris * sizeof(GLuint), &vertex_index[0], GL_STATIC_DRAW);

    f->glGenBuffers(1, &model.nbo);
    f->glBindBuffer(GL_ARRAY_BUFFER, model.nbo);
    f->glBufferData(GL_ARRAY_BUFFER, 3 * num_verts * sizeof(GLfloat), &vertex_normal[0], GL_STATIC_DRAW);

    prog->setAttributeBuffer("fnormal", GL_FLOAT, 0, 3);
    prog->enableAttributeArray("fnormal");

    f->glGenBuffers(1, &model.cbo);
    f->glBindBuffer(GL_ARRAY_BUFFER, model.cbo);
    f->glBufferData(GL_ARRAY_BUFFER, 3 * num_verts * sizeof(GLfloat), &vertex_color[0], GL_STATIC_DRAW);

    prog->setAttributeBuffer("fcolor", GL_FLOAT, 0, 3);
    prog->enableAttributeArray("fcolor");


    return model;
}

OpenGLModel OpenGLModel::FromGLTF(QOpenGLShaderProgram *prog, tinygltf::Model *gltf_model, int primIndex) {

    OpenGLModel model = OpenGLModel();
    QOpenGLFunctions_4_0_Core *f = new QOpenGLFunctions_4_0_Core();
    f->initializeOpenGLFunctions();

    //load the id of the accessor
    int colorID = gltf_model->meshes[0].primitives[primIndex].attributes["COLOR_0"];
    int normalID = gltf_model->meshes[0].primitives[primIndex].attributes["NORMAL"];
    int posID = gltf_model->meshes[0].primitives[primIndex].attributes["POSITION"];
    int indexID = gltf_model->meshes[0].primitives[primIndex].indices;

    //load the corresponding accessors
    tinygltf::Accessor colorAccessor =  gltf_model->accessors[colorID];
    tinygltf::Accessor normalAccessor =  gltf_model->accessors[normalID];
    tinygltf::Accessor posAccessor =  gltf_model->accessors[posID];
    tinygltf::Accessor indexAccessor =  gltf_model->accessors[indexID];

    //load the bufferview objects
    tinygltf::BufferView colorBV = gltf_model->bufferViews[colorAccessor.bufferView];
    tinygltf::BufferView normalBV = gltf_model->bufferViews[normalAccessor.bufferView];
    tinygltf::BufferView posBV = gltf_model->bufferViews[posAccessor.bufferView];
    tinygltf::BufferView indexBV = gltf_model->bufferViews[indexAccessor.bufferView];

    //load the buffer
    tinygltf::Buffer buff = gltf_model->buffers[posBV.buffer];

    std::vector<unsigned char> data = buff.data;

    f->glGenVertexArrays(1, &model.vao);
    f->glBindVertexArray(model.vao);

    f->glGenBuffers(1, &model.vbo);
    f->glBindBuffer(posBV.target, model.vbo);
    f->glBufferData(posBV.target, posBV.byteLength, &data[colorBV.byteOffset], GL_STATIC_DRAW);
    prog->enableAttributeArray("pos");
    prog->setAttributeBuffer("pos", posAccessor.componentType, posAccessor.byteOffset, 3, posBV.byteStride);

    f->glGenBuffers(1, &model.ibo);
    f->glBindBuffer(indexBV.target, model.ibo);
    f->glBufferData(indexBV.target, indexBV.byteLength, &data[indexBV.byteOffset], GL_STATIC_DRAW);

    f->glGenBuffers(1, &model.nbo);
    f->glBindBuffer(normalBV.target, model.nbo);
    f->glBufferData(normalBV.target, normalBV.byteLength, &data[normalBV.byteOffset], GL_STATIC_DRAW);
    prog->enableAttributeArray("fnormal");
    prog->setAttributeBuffer("fnormal", normalAccessor.componentType, normalAccessor.byteOffset, 3, normalBV.byteStride);

    f->glGenBuffers(1, &model.cbo);
    f->glBindBuffer(colorBV.target, model.cbo);
    f->glBufferData(colorBV.target, colorBV.byteLength, &data[colorBV.byteOffset], GL_STATIC_DRAW);
    prog->enableAttributeArray("fcolor");
    prog->setAttributeBuffer("fcolor", colorAccessor.componentType, colorAccessor.byteOffset, 3, colorBV.byteStride);

    model.index_type = indexAccessor.componentType;
    model.index_offset = indexAccessor.byteOffset;
    model.num_tris = indexAccessor.count / 3;

    return model;
}

void OpenGLModel::drawModel(QOpenGLContext *context) {
    QOpenGLFunctions_4_0_Core *f = new QOpenGLFunctions_4_0_Core();
    f->initializeOpenGLFunctions();

    f->glBindVertexArray(vao);

    f->glDrawElements(GL_TRIANGLES, 300 * num_tris, index_type, (void*)index_offset);


}

void OpenGLModel::setUpDrawing(QOpenGLContext *context, QOpenGLShaderProgram *program, QMatrix4x4 *viewMat) {    //for shading calculations in viewspace

    program->setUniformValue("m", model_mat);
    program->setUniformValue("normalMat", (*viewMat * model_mat).normalMatrix());

}
