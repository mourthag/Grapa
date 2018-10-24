#include "mainopenglwidget.h"
#include <qmath.h>
#define TINYGLTF_NO_STB_IMAGE_WRITE

MainOpenGLWidget::MainOpenGLWidget(QWidget *parent) : QOpenGLWidget(parent)
{
    m = QMatrix4x4();
    v = QMatrix4x4();
    p = QMatrix4x4();

    m.setToIdentity();
    v.setToIdentity();
    p.setToIdentity();
    v.lookAt(QVector3D(2,2,3), QVector3D(0,0,0), QVector3D(0,1,0));

    isWireframe = false;
    tesselation = 1;
    updateVertices();

    lightInt = 1.0;
    lightPos = QVector3D(-2.0,2.0,-1.0);

    activeProgram = &phongProgram;
}

MainOpenGLWidget::~MainOpenGLWidget() {
}

void MainOpenGLWidget::initializeGL() {

    initializeOpenGLFunctions();

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, 3 * num_verts * sizeof(GLfloat), &vertex_position[0], GL_STATIC_DRAW);

    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3 * num_tris * sizeof(GLuint), &vertex_index[0], GL_STATIC_DRAW);

    gouraudProgram = new QOpenGLShaderProgram();
    gouraudProgram->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shader/simplevertshader.vert");
    gouraudProgram->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shader/simplefragshader.frag");

    phongProgram = new QOpenGLShaderProgram();
    phongProgram->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shader/phongvertshader.vert");
    phongProgram->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shader/phongfragshader.frag");

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
    glDrawElements(GL_TRIANGLES, num_tris * 3, GL_UNSIGNED_INT, (void*)0);
}

void MainOpenGLWidget::resizeGL(int w, int h) {
    glViewport(0,0,w,h);
    width = w;
    height = h;
    p.setToIdentity();
    p.perspective(45.0, (float)w/(float)h, 0.1, 10);
    cameraUpdated();
}

void MainOpenGLWidget::mousePressEvent(QMouseEvent *event) {
    //Drag start
    if(event->buttons() == Qt::RightButton || event->buttons() == Qt::LeftButton) {
        dragStart = event->pos();
    }

}

void MainOpenGLWidget::mouseMoveEvent(QMouseEvent *event) {

    QPoint p = event->pos();
    QPointF drag = p - dragStart;

    //Translate
    if(event->buttons() == Qt::RightButton) {
        drag = 0.005 * drag;
        v.translate(drag.x(), -drag.y());
    }
    if(event->buttons() == Qt::LeftButton) {

        QPointF lastP = pixelPosToViewPos(dragStart);
        QPointF curP = pixelPosToViewPos(p);

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

        QVector3D axis = QVector3D::crossProduct(lastPos, curPos);
        float angle =  qRadiansToDegrees(std::asin(axis.length()));

        axis.normalize();

        QQuaternion q = QQuaternion::fromAxisAndAngle(axis, angle);
        v.rotate(q);
    }

    dragStart = event->pos();
    update();
    cameraUpdated();
}

void MainOpenGLWidget::wheelEvent(QWheelEvent *event) {
    float val = event->delta() / 360.0;

    QVector3D currentPos = v.inverted().map(QVector3D(0,0,0));
    QVector3D viewDir = currentPos.normalized();

    v.translate(val*viewDir);

    update();
    cameraUpdated();
}

QPointF MainOpenGLWidget::pixelPosToViewPos(const QPointF &p) {
    return QPointF(2.0 * float(p.x() / width - 1.0), 2.0 * float(p.y() / height) - 1.0);
}

void MainOpenGLWidget::resetCamera() {

    v.setToIdentity();
    v.lookAt(QVector3D(2,2,3), QVector3D(0,0,0), QVector3D(0,1,0));
    update();
    cameraUpdated();
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

    QMatrix4x4 mv = v *m;

    (*activeProgram)->bind();
    (*activeProgram)->setUniformValue("lightPos", mv.map(lightPos));
    (*activeProgram)->setUniformValue("lightInt", lightInt);
    (*activeProgram)->setUniformValue("mv", mv);
    (*activeProgram)->setUniformValue("p", p);
    (*activeProgram)->setUniformValue("normalMat", mv.normalMatrix());
    (*activeProgram)->setUniformValue("n", shininess);
}

void MainOpenGLWidget::updateVertices() {
    num_verts = (1 + tesselation)  * (1+tesselation) * 6;
    num_tris = 6 * tesselation * tesselation * 2;

    vertex_position.clear();
    vertex_color.clear();
    vertex_index.clear();
    vertex_normal.clear();

    QVector3D color;
    QVector3D normal;
    QVector3D strideA;
    QVector3D strideB;
    QVector3D start;

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
        start = normal - tesselation * 0.5 * (strideA + strideB);

        int offset = i * (tesselation+1) *(tesselation+1);

        for(int j = 0; j < tesselation+1; j++) {
            for(int k = 0; k < tesselation+1; k++) {
                QVector3D vert = start + j * strideA + k * strideB;
                vertex_position.push_back(vert.x());
                vertex_position.push_back(vert.y());
                vertex_position.push_back(vert.z());

                vertex_color.push_back(color.x());
                vertex_color.push_back(color.y());
                vertex_color.push_back(color.z());

                vertex_normal.push_back(normal.x());
                vertex_normal.push_back(normal.y());
                vertex_normal.push_back(normal.z());

                int index = offset + j * (tesselation+1) +k;
                if(j != tesselation && k != tesselation) {

                    vertex_index.push_back(index);
                    vertex_index.push_back(index + 1);
                    vertex_index.push_back(index + (tesselation+1));

                    vertex_index.push_back(index + 1);
                    vertex_index.push_back(index + 1 + (tesselation+1));
                    vertex_index.push_back(index + (tesselation+1));
                }
            }
        }
    }

}

void MainOpenGLWidget::updateVBOs() {


    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, 3 * num_verts * sizeof(GLfloat), &vertex_position[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3 * num_tris * sizeof(GLuint), &vertex_index[0], GL_STATIC_DRAW);
    gouraudProgram->setAttributeBuffer("pos", GL_FLOAT, 0, 3);
    phongProgram->setAttributeBuffer("pos", GL_FLOAT, 0, 3);

    glBindBuffer(GL_ARRAY_BUFFER, nbo);
    glBufferData(GL_ARRAY_BUFFER, 3 * num_verts * sizeof(GLfloat), &vertex_normal[0], GL_STATIC_DRAW);
    gouraudProgram->setAttributeBuffer("fnormal", GL_FLOAT, 0, 3);
    phongProgram->setAttributeBuffer("fnormal", GL_FLOAT, 0, 3);

    glBindBuffer(GL_ARRAY_BUFFER, cbo);
    glBufferData(GL_ARRAY_BUFFER, 3 * num_verts * sizeof(GLfloat), &vertex_color[0], GL_STATIC_DRAW);
    gouraudProgram->setAttributeBuffer("fcolor", GL_FLOAT, 0, 3);
    phongProgram->setAttributeBuffer("fcolor", GL_FLOAT, 0, 3);
}

void MainOpenGLWidget::loadModel(tinygltf::Model* model) {
//    int colorID = model->meshes[0].primitives[0].attributes["COLOR_0"];
//    int normalID = model->meshes[0].primitives[0].attributes["NORMAL"];
//    int posID = model->meshes[0].primitives[0].attributes["POSITION"];
//    int indexID = model->meshes[0].primitives[0].indices;

//    tinygltf::Accessor colorAccessor =  model->accessors[colorID];
//    tinygltf::Accessor normalAccessor =  model->accessors[normalID];
//    tinygltf::Accessor posAccessor =  model->accessors[posID];
//    tinygltf::Accessor indexAccessor =  model->accessors[indexID];

//    tinygltf::BufferView colorBV = model->bufferViews[colorAccessor.bufferView];
//    tinygltf::BufferView nomalBV = model->bufferViews[normalAccessor.bufferView];
//    tinygltf::BufferView posBV = model->bufferViews[posAccessor.bufferView];
//    tinygltf::BufferView indexBV = model->bufferViews[indexAccessor.bufferView];

//    tinygltf::Buffer buff = model->buffers[posBV.buffer];


//    glBindBuffer(GL_ARRAY_BUFFER, vbo);
//    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
//    glBufferData(posBV.target, posBV.byteLength, &buff.data[0], GL_STATIC_DRAW);

//    GLint posLoc = glGetAttribLocation(*activeProgram, "pos");

//    glVertexAttribPointer(posLoc, posBV.byteLength, posAccessor.componentType, GL_FALSE, posBV.byteStride, posBV.byteOffset );
//    glBindAttribLocation(*activeProgram, posLoc, "pos");
//    glEnableVertexAttribArray(posLoc);

//    glBufferData(indexBV.target, indexBV.byteLength, &buff.data[0], GL_STATIC_DRAW);
}

QMatrix4x4 MainOpenGLWidget::getViewMat() {
    return v;
}


