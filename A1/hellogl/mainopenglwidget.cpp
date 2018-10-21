#include "mainopenglwidget.h"
#include <QFileInfo>

const unsigned int num_verts = 4 ;
const unsigned int num_tris = 2 ;

const GLfloat vertex_position[ 3*num_verts] =
{
1.0, 1.0, 0.0,
-1.0 , 1.0 , 0.0 ,
-1.0 , -1.0 , 0.0 ,
1.0 , -1.0 , 0.0
};

const GLuint vertex_index[ 3*num_tris] =
{
0 , 1 , 2 ,
0 , 2 , 3
};

MainOpenGLWidget::MainOpenGLWidget(QWidget *parent) : QOpenGLWidget(parent)
{

}

MainOpenGLWidget::~MainOpenGLWidget() {
}

void MainOpenGLWidget::initializeGL() {

    m = QMatrix4x4();
    v = QMatrix4x4();
    p = QMatrix4x4();


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

    program->bind();
    program->setUniformValue("color", QVector3D(0.0, 1.0, 0.0));
}

void MainOpenGLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m.setToIdentity();
    v.setToIdentity();
    v.lookAt(QVector3D(2,2,3), QVector3D(0,0,0), QVector3D(0,1,0));

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

void MainOpenGLWidget::mousePressEvent(QMouseEvent *event) {}
void MainOpenGLWidget::mouseMoveEvent(QMouseEvent *event) {}
void MainOpenGLWidget::wheelEvent(QWheelEvent *event) {}


