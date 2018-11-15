#ifndef MAINOPENGLWIDGET_H
#define MAINOPENGLWIDGET_H

#include <QOpenGLWidget>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QOpenGLFunctions_4_0_Core>
#include <QOpenGLShaderProgram>
#include <QMatrix4x4>
#include <QQuaternion>
#include <QTimer>

#include <scenerenderer.h>
#include <performancechart.h>
#include <scene.h>

#define TINYGLTF_NO_STB_IMAGE_WRITE
#include <tiny_gltf.h>

class MainOpenGLWidget : public QOpenGLWidget, protected QOpenGLFunctions_4_0_Core
{
    Q_OBJECT

public:
    MainOpenGLWidget(QWidget *parent = 0);
    ~MainOpenGLWidget();


public slots:
    void resetCamera();
    void setShininess(int s);
    void setLightIntensity(int i);
    void setTesselation(int t);
    void setWireframe();
    void setGouraud();
    void setPhong();
    void setLightPos(QVector3D v);
    void setDeferredPhong();
    void setDeferredNormal();
    void setDeferredUV();
    void setDeferredViewPos();
    void setDeferredMaterial();
    void playAnimation();
    void pauseAnimation();
    void loadModel(tinygltf::Model *gltf_model);
    void loadTerrain(QFile *pgmFile);

    QChartView* getChartView();
signals:
    void cameraUpdated(QMatrix4x4 *viewMat);

protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int width, int height) override;
    void mousePressEvent(QMouseEvent *event ) override;
    void mouseMoveEvent(QMouseEvent *event ) override;
    void wheelEvent(QWheelEvent *event ) override;

    QPointF pixelPosToViewPos(const QPointF& point);
private:
    Scene scene;
    SceneRenderer renderer;

    int width;
    int height;
    int shininess;

    int tesselation;
    bool isWireframe;
    bool modelLoaded;

    QPointF dragStart;
};

#endif // MAINOPENGLWIDGET_H
