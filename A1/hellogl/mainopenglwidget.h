#ifndef MAINOPENGLWIDGET_H
#define MAINOPENGLWIDGET_H

#include <QOpenGLWidget>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QOpenGLFunctions_4_0_Core>
#include <QOpenGLShaderProgram>
#include <QMatrix4x4>
#include <QQuaternion>

#define TINYGLTF_NO_STB_IMAGE_WRITE
#include <tiny_gltf.h>

class MainOpenGLWidget : public QOpenGLWidget, protected QOpenGLFunctions_4_0_Core
{
    Q_OBJECT

public:
    MainOpenGLWidget(QWidget *parent = 0);
    ~MainOpenGLWidget();

    QMatrix4x4 getViewMat();
    void loadModel(tinygltf::Model *model);
public slots:
    void resetCamera();
    void setShininess(int s);
    void setLightIntensity(int i);
    void setTesselation(int t);
    void setWireframe();
    void setGouraud();
    void setPhong();
    void setLightPos(QVector3D v);

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
    void updateVertices();
    void updateVBOs();

    QPointF pixelPosToViewPos(const QPointF& point);
private:    
    int width;
    int height;

    QVector3D lightPos;
    float lightInt;
    int shininess;

    int tesselation;
    bool isWireframe;
    bool modelLoaded;

    size_t offset;

    QMatrix4x4 m;
    QMatrix4x4 v;
    QMatrix4x4 p;

    QPointF dragStart;

    QOpenGLShaderProgram **activeProgram;
    QOpenGLShaderProgram *phongProgram;
    QOpenGLShaderProgram *gouraudProgram;

    int num_tris;
    int num_verts;

    std::vector<GLfloat> vertex_position;
    std::vector<GLuint> vertex_index;
    std::vector<GLfloat> vertex_color;
    std::vector<GLfloat> vertex_normal;

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
