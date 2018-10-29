#include "mainopenglwidget.h"
#include <qmath.h>
#define TINYGLTF_NO_STB_IMAGE_WRITE

MainOpenGLWidget::MainOpenGLWidget(QWidget *parent) : QOpenGLWidget(parent)
{
    //set up matrices
    m = QMatrix4x4();
    v = QMatrix4x4();
    p = QMatrix4x4();

    m.setToIdentity();
    v.setToIdentity();
    p.setToIdentity();
    v.lookAt(QVector3D(2, 2, 3), QVector3D(0,0,0), QVector3D(0,1,0));

    //set up data and shading
    isWireframe = false;
    modelLoaded = false;
    tesselation = 1;
    updateVertices();

    //set up lightning
    lightInt = 1.0;
    lightPos = QVector3D(-2.0,2.0,-1.0);

    activeProgram = &phongProgram;
}

MainOpenGLWidget::~MainOpenGLWidget() {
    glDisableVertexAttribArray(vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ibo);
    glDeleteBuffers(1, &cbo);
    glDeleteBuffers(1, &nbo);
}


void MainOpenGLWidget::initializeGL() {

    initializeOpenGLFunctions();

    gouraudProgram = new QOpenGLShaderProgram();
    gouraudProgram->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shader/gouraudvertshader.vert");
    gouraudProgram->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shader/gouraudfragshader.frag");

    phongProgram = new QOpenGLShaderProgram();
    phongProgram->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shader/phongvertshader.vert");
    phongProgram->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shader/phongfragshader.frag");

    //new VAO
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    //For each vbo generate buffer, buffer data, enable attribute array and set attribute layout
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, 3 * num_verts * sizeof(GLfloat), &vertex_position[0], GL_STATIC_DRAW);

    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3 * num_tris * sizeof(GLuint), &vertex_index[0], GL_STATIC_DRAW);

    gouraudProgram->link();
    gouraudProgram->enableAttributeArray("pos");
    gouraudProgram->setAttributeBuffer("pos", GL_FLOAT, 0, 3);

    phongProgram->link();
    phongProgram->enableAttributeArray("pos");
    phongProgram->setAttributeBuffer("pos", GL_FLOAT, 0, 3);

    glGenBuffers(1, &nbo);
    glBindBuffer(GL_ARRAY_BUFFER, nbo);
    glBufferData(GL_ARRAY_BUFFER, 3 * num_verts * sizeof(GLfloat), &vertex_normal[0], GL_STATIC_DRAW);

    gouraudProgram->enableAttributeArray("fnormal");
    gouraudProgram->setAttributeBuffer("fnormal", GL_FLOAT, 0, 3);
    phongProgram->enableAttributeArray("fnormal");
    phongProgram->setAttributeBuffer("fnormal", GL_FLOAT, 0, 3);

    glGenBuffers(1, &cbo);
    glBindBuffer(GL_ARRAY_BUFFER, cbo);
    glBufferData(GL_ARRAY_BUFFER, 3 * num_verts * sizeof(GLfloat), &vertex_color[0], GL_STATIC_DRAW);

    gouraudProgram->enableAttributeArray("fcolor");
    gouraudProgram->setAttributeBuffer("fcolor", GL_FLOAT, 0, 3);
    phongProgram->enableAttributeArray("fcolor");
    phongProgram->setAttributeBuffer("fcolor", GL_FLOAT, 0, 3);

}

void MainOpenGLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    if(isWireframe) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    updateUniforms();

    glDrawElements(GL_TRIANGLES, num_tris * 3, index_type, (void*)0);
}

void MainOpenGLWidget::resizeGL(int w, int h) {
    //update viewport
    glViewport(0,0,w,h);

    //for trackball calculations
    width = w;
    height = h;

    //projection matrix
    p.setToIdentity();
    p.perspective(45.0, (float)w/(float)h, 0.1, 10);
    cameraUpdated(&v);
}

void MainOpenGLWidget::mousePressEvent(QMouseEvent *event) {
    //Drag start
    if(event->buttons() == Qt::RightButton || event->buttons() == Qt::LeftButton) {
        dragStart = event->pos();
    }

}

void MainOpenGLWidget::mouseMoveEvent(QMouseEvent *event) {

    QPoint p = event->pos();

    //Translate
    if(event->buttons() == Qt::RightButton) {
        QPointF mappedP = v.map(p);
        QPointF mappedLastP = v.map(dragStart);

        QPointF drag = mappedP - mappedLastP;

        drag = 0.005 * drag;
        v.translate(drag.x(), -drag.y());
    }
    //rotate
    if(event->buttons() == Qt::LeftButton) {

        QPointF lastP = pixelPosToViewPos(dragStart);
        QPointF curP = pixelPosToViewPos(p);

        //calculate 3d positions on the plane

        QVector3D lastPos = QVector3D(lastP.x(), lastP.y(), 0.0);
        float sqrZ = QVector3D::dotProduct(lastPos, lastPos);
        if(sqrZ  <= 1.0/2.0)
            lastPos.setZ(std::sqrt(1 - sqrZ));
        else
            lastPos.setZ((1.0/2.0)/std::sqrt(sqrZ));


        QVector3D curPos = QVector3D(curP.x(), curP.y(), 0.0);
        sqrZ = QVector3D::dotProduct(curPos, curPos);

        if(sqrZ  <= 1.0/2.0)
            curPos.setZ(std::sqrt(1 - sqrZ));
        else
            curPos.setZ((1.0/2.0)/std::sqrt(sqrZ));

        //calculate rotation axis and angle
        QVector3D axis = QVector3D::crossProduct(lastPos, curPos);
        float angle =  qRadiansToDegrees(std::asin(axis.length()));

        //transform rotation vector with current rotation
        axis = v.mapVector(axis);
        axis.normalize();

        //create quaternion and rotate with it
        QQuaternion q = QQuaternion::fromAxisAndAngle(axis, angle);
        v.rotate(q);
    }

    dragStart = event->pos();
    //update screen and camera/statusbar
    update();
    cameraUpdated(&v);
}

void MainOpenGLWidget::wheelEvent(QWheelEvent *event) {
    //slow zooming down, cause delta is in degrees
    float val = event->delta() / 360.0;

    //calculate view direction
    QVector3D currentPos = v.inverted().map(QVector3D(0,0,0));
    QVector3D viewDir = currentPos.normalized();

    //translate along viewdir
    v.translate(val*viewDir);

    //update screen etc
    update();
    cameraUpdated(&v);
}

QPointF MainOpenGLWidget::pixelPosToViewPos(const QPointF &p) {
    return QPointF(2.0 * float(p.x() / width - 1.0), 2.0 * float(p.y() / height) - 1.0);
}

void MainOpenGLWidget::resetCamera() {

    //restore initial view matrix and update screen
    v.setToIdentity();
    v.lookAt(QVector3D(2,2,3), QVector3D(0,0,0), QVector3D(0,1,0));
    update();
    cameraUpdated(&v);
}

void MainOpenGLWidget::setShininess(int s) {
    shininess = s;
    update();
}

void MainOpenGLWidget::setTesselation(int t) {
    tesselation = t;
    updateVertices();
    updateVBOs();
    update();
}

void MainOpenGLWidget::setWireframe() {
    isWireframe = true;
    update();
}

void MainOpenGLWidget::setLightPos(QVector3D v) {
    lightPos = v;
    update();
}

void MainOpenGLWidget::setLightIntensity(int i) {
    lightInt = (float)i/10.0;
    update();
}

void MainOpenGLWidget::setPhong() {

    isWireframe = false;
    activeProgram = &phongProgram;
    update();
}

void MainOpenGLWidget::setGouraud() {

    isWireframe = false;
    activeProgram = &gouraudProgram;
    update();
}

void MainOpenGLWidget::updateUniforms() {

    //for shading calculations in viewspace
    QMatrix4x4 mv = v *m;

    //update uniform values
    (*activeProgram)->bind();
    (*activeProgram)->setUniformValue("lightPos", mv.map(lightPos));
    (*activeProgram)->setUniformValue("lightInt", lightInt);
    (*activeProgram)->setUniformValue("mv", mv);
    (*activeProgram)->setUniformValue("p", p);
    (*activeProgram)->setUniformValue("normalMat", mv.normalMatrix());
    (*activeProgram)->setUniformValue("n", shininess);
}

void MainOpenGLWidget::updateVertices() {
    modelLoaded = false;
    index_type = GL_UNSIGNED_INT;

    //calculate number of vertices and number of triangles
    //each side has its own vertices and triangles
    num_verts = (1 + tesselation)  * (1+tesselation) * 6;
    num_tris = 6 * tesselation * tesselation * 2;

    vertex_position.clear();
    vertex_color.clear();
    vertex_index.clear();
    vertex_normal.clear();

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

}

void MainOpenGLWidget::updateVBOs() {

    glBindVertexArray(vao);

    //Buffer the new data so open gl will draw with this
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, 3 * num_verts * sizeof(GLfloat), &vertex_position[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3 * num_tris * sizeof(GLuint), &vertex_index[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, nbo);
    glBufferData(GL_ARRAY_BUFFER, 3 * num_verts * sizeof(GLfloat), &vertex_normal[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, cbo);
    glBufferData(GL_ARRAY_BUFFER, 3 * num_verts * sizeof(GLfloat), &vertex_color[0], GL_STATIC_DRAW);
}

void MainOpenGLWidget::loadModel(tinygltf::Model* model) {

    (*activeProgram)->bind();
    glBindVertexArray(vao);
    modelLoaded = true;

    //load the id of the accessor
    int colorID = model->meshes[0].primitives[0].attributes["COLOR_0"];
    int normalID = model->meshes[0].primitives[0].attributes["NORMAL"];
    int posID = model->meshes[0].primitives[0].attributes["POSITION"];
    int indexID = model->meshes[0].primitives[0].indices;

    //load the corresponding accessors
    tinygltf::Accessor colorAccessor =  model->accessors[colorID];
    tinygltf::Accessor normalAccessor =  model->accessors[normalID];
    tinygltf::Accessor posAccessor =  model->accessors[posID];
    tinygltf::Accessor indexAccessor =  model->accessors[indexID];

    //load the bufferview objects
    tinygltf::BufferView colorBV = model->bufferViews[colorAccessor.bufferView];
    tinygltf::BufferView normalBV = model->bufferViews[normalAccessor.bufferView];
    tinygltf::BufferView posBV = model->bufferViews[posAccessor.bufferView];
    tinygltf::BufferView indexBV = model->bufferViews[indexAccessor.bufferView];

    //load the buffer
    tinygltf::Buffer buff = model->buffers[posBV.buffer];

    std::vector<GLfloat> pos_data;
    std::vector<GLfloat> nor_data;
    std::vector<GLfloat> col_data;

    index_type = indexAccessor.componentType;

    //convert and split the buffer to their respective vertex attributes
    //I really tried achieving this through glVertexAttribPointer or setAttributeBuffer but couldn't get it to work in any way
    convertBuffer(3, posAccessor.byteOffset, posBV.byteStride, posBV.byteLength, &buff.data, &pos_data);
    convertBuffer(3, normalAccessor.byteOffset, normalBV.byteStride, normalBV.byteLength, &buff.data, &nor_data);
    convertBuffer(3, colorAccessor.byteOffset, colorBV.byteStride, colorBV.byteLength, &buff.data, &col_data);

    //Buffer the new data
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ARRAY_BUFFER, pos_data.size() * sizeof(GLfloat), &pos_data[0], GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexBV.byteLength, &buff.data[indexBV.byteOffset], GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, nbo);
    glBufferData(GL_ARRAY_BUFFER, nor_data.size() * sizeof(GLfloat) , &nor_data[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, cbo);
    glBufferData(GL_ARRAY_BUFFER, col_data.size() * sizeof(GLfloat), &col_data[0], GL_STATIC_DRAW);

    num_tris = indexAccessor.count ;
    update();


}

void MainOpenGLWidget::convertBuffer(int size, int offset, int stride, int length, std::vector<unsigned char> *data, std::vector<GLfloat> *convertedData) {

    for(int i=offset; i < length; i += stride) {
        //map memory from the binary buffer to an array of GLfloats
        GLfloat value[size];
        mempcpy(&value, &(*data)[i], size * sizeof(GLfloat));

        //push values of array to the new buffer
        for(int j = 0; j < size; j++) {
            convertedData->push_back(value[j]);
        }
    }

}


