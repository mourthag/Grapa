#ifndef MAINOPENGLWIDGET_H
#define MAINOPENGLWIDGET_H

#include <QOpenGLWidget>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QOpenGLShaderProgram>
#include <QMatrix4x4>
#include <QQuaternion>
#include <QTimer>

#include "util/performancechart.h"
#include "util/openglfunctions.h"

#include "scenegraph/scenerenderer.h"
#include "scenegraph/scene.h"

#include "terrain/terrainscene.h"
#include "terrain/terrainscenerenderer.h"

#include "snowterrain/snowterrainscene.h"
#include "snowterrain/snowterrainscenerenderer.h"

#define TINYGLTF_NO_STB_IMAGE_WRITE
#include <tiny_gltf.h>

class MainOpenGLWidget : public QOpenGLWidget
{
    Q_OBJECT

public:
    MainOpenGLWidget(QWidget *parent = 0);
    ~MainOpenGLWidget();

    QChartView* getChartView();
    SnowTerrainScene terrainScene;
    SnowTerrainSceneRenderer terrainRenderer;

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
    void loadSkybox(QString dir);

    void setForrestData(ForrestData data);
    void setTerrainDrawEnabled(bool val);
    void setTreeDrawEnabled(bool val);
    void setSkyboxDrawEnabled(bool val);
    void setFrustumCullingEnabled(bool val);
    void setNumImpostors(int val);




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


    int width;
    int height;
    int shininess;

    bool isWireframe;

    QPointF dragStart;
};

#endif // MAINOPENGLWIDGET_H
