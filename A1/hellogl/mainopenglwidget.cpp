#include "mainopenglwidget.h"
#include <qmath.h>
#include <cmath>

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

    quat = QQuaternion();

    m.setToIdentity();
    v.setToIdentity();
    p.setToIdentity();
    v.lookAt(QVector3D(2,2,3), QVector3D(0,0,0), QVector3D(0,1,0));


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

    program = new QOpenGLShaderProgram();
    program->addShaderFromSourceFile(QOpenGLShader::Vertex, "simplevertshader.vert");
    program->addShaderFromSourceFile(QOpenGLShader::Fragment, "simplefragshader.frag");

    program->link();
    program->enableAttributeArray("pos");
    program->setAttributeBuffer("pos", GL_FLOAT, 0, 3);

    glGenBuffers(1, &nbo);
    glBindBuffer(GL_ARRAY_BUFFER, nbo);
    glBufferData(GL_ARRAY_BUFFER, 3 * num_verts * sizeof(GLfloat), face_normal, GL_STATIC_DRAW);

    program->enableAttributeArray("fnormal");
    program->setAttributeBuffer("fnormal", GL_FLOAT, 0, 3);

    glGenBuffers(1, &cbo);
    glBindBuffer(GL_ARRAY_BUFFER, cbo);
    glBufferData(GL_ARRAY_BUFFER, 3 * num_verts * sizeof(GLfloat), face_color, GL_STATIC_DRAW);

    program->enableAttributeArray("fcolor");
    program->setAttributeBuffer("fcolor", GL_FLOAT, 0, 3);

    program->bind();
}

void MainOpenGLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    QMatrix4x4 mvp = p * v *m;
    program->setUniformValue("mvp", mvp);
    program->bind();
    glDrawElements(GL_TRIANGLES, num_tris * 3, GL_UNSIGNED_INT, (void*)0);
}

void MainOpenGLWidget::resizeGL(int w, int h) {
    glViewport(0,0,w,h);
    p.setToIdentity();
    p.perspective(45.0, (float)w/(float)h, 0.1, 10);
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

        QVector3D lastPos = QVector3D(dragStart.x(), dragStart.y(), 0.0);
        float sqrZ = 1 - QVector3D::dotProduct(lastPos, lastPos);
        if(sqrZ > 0)
            lastPos.setZ(std::sqrt(sqrZ));
        else
            lastPos.normalize();


        QVector3D curPos = QVector3D(p.x(), p.y(), 0.0);
        sqrZ = 1 - QVector3D::dotProduct(curPos, curPos);
        if(sqrZ > 0)
            curPos.setZ(std::sqrt(sqrZ));
        else
            curPos.normalize();

        QVector3D axis = QVector3D::crossProduct(lastPos, curPos);
        float angle =  qRadiansToDegrees(std::asin(axis.length()));

        axis.normalize();
        axis = quat.rotatedVector(axis);

        quat = QQuaternion::fromAxisAndAngle(axis, angle) * quat;
        v.rotate(quat);
    }

    dragStart = event->pos();
    update();
}

void MainOpenGLWidget::wheelEvent(QWheelEvent *event) {
    float val = event->delta() / 360.0;

    QVector3D currentPos = v.inverted().map(QVector3D(0,0,0));
    QVector3D viewDir = currentPos.normalized();

    v.translate(val*viewDir);

    update();
}


