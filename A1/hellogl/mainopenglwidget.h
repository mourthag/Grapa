#ifndef MAINOPENGLWIDGET_H
#define MAINOPENGLWIDGET_H

#include <QOpenGLWidget>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QOpenGLFunctions_4_0_Core>
#include <QOpenGLShaderProgram>
#include <QMatrix4x4>
#include <QQuaternion>

class MainOpenGLWidget : public QOpenGLWidget, protected QOpenGLFunctions_4_0_Core
{
    Q_OBJECT

public:
    MainOpenGLWidget(QWidget *parent = 0);
    ~MainOpenGLWidget();

protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int width, int height) override;
    void mousePressEvent(QMouseEvent *event ) override;
    void mouseMoveEvent(QMouseEvent *event ) override;
    void wheelEvent(QWheelEvent *event ) override;

private:
    QMatrix4x4 m;
    QMatrix4x4 v;
    QMatrix4x4 p;

    QPointF dragStart;
    QQuaternion quat;

    QOpenGLShaderProgram *program;
    GLuint vao;
    //vertices
    GLuint vbo;
    //indices
    GLuint ibo;
    //normals
    GLuint nbo;
    //colors
    GLuint cbo;
};

#endif // MAINOPENGLWIDGET_H
