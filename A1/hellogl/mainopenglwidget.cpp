#include "mainopenglwidget.h"
#include <qmath.h>

const unsigned int num_verts = 3 * 8 ;
const unsigned int num_faces = 6 ;
const unsigned int num_tris = 2 * num_faces ;

const GLfloat vertex_position[ 3 *  num_verts] =
{
    1.0, 1.0, -1.0,
    -1.0 , 1.0 , -1.0 ,
    -1.0 , -1.0 , -1.0 ,
    1.0 , -1.0 , -1.0,
    1.0, 1.0, 1.0,
    -1.0 , 1.0 , 1.0 ,
    -1.0 , -1.0 , 1.0 ,
    1.0 , -1.0 , 1.0,
    1.0, 1.0, -1.0,
    -1.0 , 1.0 , -1.0 ,
    -1.0 , -1.0 , -1.0 ,
    1.0 , -1.0 , -1.0,
    1.0, 1.0, 1.0,
    -1.0 , 1.0 , 1.0 ,
    -1.0 , -1.0 , 1.0 ,
    1.0 , -1.0 , 1.0,
    1.0, 1.0, -1.0,
    -1.0 , 1.0 , -1.0 ,
    -1.0 , -1.0 , -1.0 ,
    1.0 , -1.0 , -1.0,
    1.0, 1.0, 1.0,
    -1.0 , 1.0 , 1.0 ,
    -1.0 , -1.0 , 1.0 ,
    1.0 , -1.0 , 1.0

};

const GLuint vertex_index[ 3*num_tris] =
{
    //front
    4 , 5 , 6 ,
    4 , 6 , 7 ,
    //behind
    2 , 1 , 0 ,
    3 , 2 , 0 ,
    //top
    8 , 9 , 13 ,
    8 , 13 , 12 ,
    //bottom
    10 , 11 , 14 ,
    15 , 14 , 11 ,
    //right
    16 , 20 , 23 ,
    16 , 23 , 19 ,
    //left
    21 , 17 , 18 ,
    21 , 18 , 22
};

const GLfloat face_normal[3 *  num_verts] =
{
    0.0, 0.0, -1.0,
    0.0, 0.0, -1.0,
    0.0, 0.0, -1.0,
    0.0, 0.0, -1.0,
    0.0, 0.0, 1.0,
    0.0, 0.0, 1.0,
    0.0, 0.0, 1.0,
    0.0, 0.0, 1.0,
    0.0, 1.0, 0.0,
    0.0, 1.0, 0.0,
    0.0, -1.0, 0.0,
    0.0, -1.0, 0.0,
    0.0, 1.0, 0.0,
    0.0, 1.0, 0.0,
    0.0, -1.0, 0.0,
    0.0, -1.0, 0.0,
    1.0, 0.0, 0.0,
    -1.0, 0.0, 0.0,
    -1.0, 0.0, 0.0,
    1.0, 0.0, 0.0,
    1.0, 0.0, 0.0,
    -1.0, 0.0, 0.0,
    -1.0, 0.0, 0.0,
    1.0, 0.0, 0.0
};

const GLfloat face_color[3 * num_verts] =
{
    1.0, 0.0, 0.0,
    1.0, 0.0, 0.0,
    1.0, 0.0, 0.0,
    1.0, 0.0, 0.0,
    0.0, 1.0, 0.0,
    0.0, 1.0, 0.0,
    0.0, 1.0, 0.0,
    0.0, 1.0, 0.0,
    0.0, 0.0, 1.0,
    0.0, 0.0, 1.0,
    0.5, 0.0, 0.5,
    0.5, 0.0, 0.5,
    0.0, 0.0, 1.0,
    0.0, 0.0, 1.0,
    0.5, 0.0, 0.5,
    0.5, 0.0, 0.5,
    0.5, 0.5, 0.0,
    0.0, 0.5, 0.5,
    0.0, 0.5, 0.5,
    0.5, 0.5, 0.0,
    0.5, 0.5, 0.0,
    0.0, 0.5, 0.5,
    0.0, 0.5, 0.5,
    0.5, 0.5, 0.0
};

MainOpenGLWidget::MainOpenGLWidget(QWidget *parent) : QOpenGLWidget(parent)
{
    m = QMatrix4x4();
    v = QMatrix4x4();
    p = QMatrix4x4();

    m.setToIdentity();
    v.setToIdentity();
    p.setToIdentity();
    v.lookAt(QVector3D(2,2,3), QVector3D(0,0,0), QVector3D(0,1,0));

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
    glBufferData(GL_ARRAY_BUFFER, 3 * num_verts * sizeof(GLfloat), vertex_position, GL_STATIC_DRAW);

    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3 * num_tris * sizeof(GLuint), vertex_index, GL_STATIC_DRAW);

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
    glBufferData(GL_ARRAY_BUFFER, 3 * num_verts * sizeof(GLfloat), face_normal, GL_STATIC_DRAW);

    gouraudProgram->enableAttributeArray("fnormal");
    gouraudProgram->setAttributeBuffer("fnormal", GL_FLOAT, 0, 3);
    phongProgram->enableAttributeArray("fnormal");
    phongProgram->setAttributeBuffer("fnormal", GL_FLOAT, 0, 3);

    glGenBuffers(1, &cbo);
    glBindBuffer(GL_ARRAY_BUFFER, cbo);
    glBufferData(GL_ARRAY_BUFFER, 3 * num_verts * sizeof(GLfloat), face_color, GL_STATIC_DRAW);

    gouraudProgram->enableAttributeArray("fcolor");
    gouraudProgram->setAttributeBuffer("fcolor", GL_FLOAT, 0, 3);
    phongProgram->enableAttributeArray("fcolor");
    phongProgram->setAttributeBuffer("fcolor", GL_FLOAT, 0, 3);

}

void MainOpenGLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

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

void MainOpenGLWidget::setWireframe() {

}

void MainOpenGLWidget::setPhong() {

    activeProgram = &phongProgram;
    update();
}

void MainOpenGLWidget::setGouraud() {

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

QMatrix4x4 MainOpenGLWidget::getViewMat() {
    return v;
}


