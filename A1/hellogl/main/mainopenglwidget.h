#ifndef MAINOPENGLWIDGET_H
#define MAINOPENGLWIDGET_H

#include <QOpenGLWidget>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QOpenGLFunctions_4_3_Core>
#include <QOpenGLShaderProgram>
#include <QMatrix4x4>
#include <QQuaternion>
#include <QTimer>

#include "scenegraph/scenerenderer.h"
#include "util/performancechart.h"
#include "scenegraph/scene.h"

#include "terrain/terrainscene.h"
#include "terrain/terrainscenerenderer.h"

#define TINYGLTF_NO_STB_IMAGE_WRITE
#include <tiny_gltf.h>

class MainOpenGLWidget : public QOpenGLWidget, protected QOpenGLFunctions_4_3_Core
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
    void setHeightScaling(int scaling);
    void setPhong();
    void setLightPos(QVector3D v);
    void setLightCol(QVector3D color);
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
    void cameraUpdated(QMatrix4x4 viewMat);

protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int width, int height) override;
    void mousePressEvent(QMouseEvent *event ) override;
    void mouseMoveEvent(QMouseEvent *event ) override;
    void wheelEvent(QWheelEvent *event ) override;
    void keyPressEvent(QKeyEvent *event) override;

    QPointF pixelPosToViewPos(const QPointF& point);
private:
    //Scene scene;
    //SceneRenderer renderer;

    TerrainScene terrainScene;
    TerrainSceneRenderer terrainRenderer;

    int width;
    int height;
    int shininess;

    bool isWireframe;

    QPointF dragStart;
};

#endif // MAINOPENGLWIDGET_H
