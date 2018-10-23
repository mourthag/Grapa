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

    QMatrix4x4 getViewMat();
public slots:
    void resetCamera();
    void setShininess(int s);
    void setWireframe();
    void setGouraud();
    void setPhong();

signals:
    void cameraUpdated();

protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int width, int height) override;
    void mousePressEvent(QMouseEvent *event ) override;
    void mouseMoveEvent(QMouseEvent *event ) override;
    void wheelEvent(QWheelEvent *event ) override;

    void updateUniforms();

    QPointF pixelPosToViewPos(const QPointF& point);
private:    
    int width;
    int height;

    QVector3D lightPos;
    float lightInt;
    int shininess;

    QMatrix4x4 m;
    QMatrix4x4 v;
    QMatrix4x4 p;

    QPointF dragStart;

    QOpenGLShaderProgram **activeProgram;
    QOpenGLShaderProgram *phongProgram;
    QOpenGLShaderProgram *gouraudProgram;

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
