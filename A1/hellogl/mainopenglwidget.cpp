#include "mainopenglwidget.h"
#include <qmath.h>
#define TINYGLTF_NO_STB_IMAGE_WRITE

MainOpenGLWidget::MainOpenGLWidget(QWidget *parent) : QOpenGLWidget(parent)
{

    setFocusPolicy(Qt::StrongFocus);


    //update at ~60FPS
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(update()));
    timer->start(16);


    //set up data and shading
    isWireframe = false;
    modelLoaded = false;
    tesselation = 1;

}

MainOpenGLWidget::~MainOpenGLWidget() {
}


void MainOpenGLWidget::initializeGL() {

    initializeOpenGLFunctions();


    makeCurrent();
    scene.initGL();
    makeCurrent();
    renderer.initGL();
    doneCurrent();

    scene.getCameraLightInfo()->lightInt = 1;
    scene.getCameraLightInfo()->lightPos = QVector3D(0,0,0);

    renderer.setRenderMode(SceneRenderer::Phong);

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



    renderer.drawScene(&scene);


}

void MainOpenGLWidget::resizeGL(int w, int h) {
    //update viewport
    glViewport(0,0,w,h);

    makeCurrent();
    renderer.updateFramebuffeSize(w, h);

    doneCurrent();

    CameraLightInfo *cam = scene.getCameraLightInfo();

    //for trackball calculations
    width = w;
    height = h;

    //projection matrix
    cam->projMatrix.setToIdentity();
    cam->projMatrix.perspective(45.0, (float)w/(float)h, 0.1, 10000);
    cameraUpdated(cam->viewMatrix());
}

void MainOpenGLWidget::mousePressEvent(QMouseEvent *event) {
    //Drag start
    if(event->buttons() == Qt::RightButton || event->buttons() == Qt::LeftButton) {
        dragStart = event->pos();
    }

}

void MainOpenGLWidget::mouseMoveEvent(QMouseEvent *event) {

    QPoint p = event->pos();

    CameraLightInfo *cam = scene.getCameraLightInfo();

    //Translate
    if(event->buttons() == Qt::RightButton) {


        QPointF drag = pixelPosToViewPos( p) - pixelPosToViewPos( dragStart);
        drag.setY(-drag.y());


        QVector3D dragVec = cam->camRotation.inverted().mapVector(QVector3D(drag));
        cam->camTranslation += dragVec;
    }
    //rotate
    if(event->buttons() == Qt::LeftButton) {

        QPointF lastP = pixelPosToViewPos(dragStart);
        QPointF curP = pixelPosToViewPos(p);

        //calculate 3d positions on the plane

        QVector3D lastPos = QVector3D(lastP.x(), -lastP.y(), 0.0);
        float sqrZ = QVector3D::dotProduct(lastPos, lastPos);
        if(sqrZ  <= 1.0/2.0)
            lastPos.setZ(std::sqrt(1 - sqrZ));
        else
            lastPos.setZ((1.0/2.0)/std::sqrt(sqrZ));


        QVector3D curPos = QVector3D(curP.x(), -curP.y(), 0.0);
        sqrZ = QVector3D::dotProduct(curPos, curPos);

        if(sqrZ  <= 1.0/2.0)
            curPos.setZ(std::sqrt(1 - sqrZ));
        else
            curPos.setZ((1.0/2.0)/std::sqrt(sqrZ));

        //calculate rotation axis and angle
        QVector3D axis = QVector3D::crossProduct(lastPos, curPos);
        float angle =  qRadiansToDegrees(std::asin(axis.length()));

        //transform rotation vector with current rotation
        axis = cam->camRotation.inverted().mapVector(axis);
        axis.normalize();

        //create quaternion and rotate with it
        QQuaternion q = QQuaternion::fromAxisAndAngle(axis, angle);
        cam->camRotation.rotate(q);
    }

    dragStart = event->pos();
    //update screen and camera/statusbar
    update();
    cameraUpdated(cam->viewMatrix());
}

void MainOpenGLWidget::wheelEvent(QWheelEvent *event) {
    //slow zooming down, cause delta is in degrees
    float val = event->delta() / 360.0;

    CameraLightInfo *cam = scene.getCameraLightInfo();

    //calculate view direction
    QVector3D currentPos = cam->camTranslation;
    QVector3D viewDir = currentPos.normalized();

    //translate along viewdir
    cam->camTranslation += val*viewDir;

    //update screen etc
    update();
    cameraUpdated(cam->viewMatrix());
}

void MainOpenGLWidget::keyPressEvent(QKeyEvent *event) {
    //QOpenGLWidget::keyPressEvent(event);
    int key = event->key();

    CameraLightInfo *cam = scene.getCameraLightInfo();

    QVector3D forward = cam->camRotation.inverted().mapVector(QVector3D(0,0,1));
    forward.setY(0);
    QVector3D right = cam->camRotation.inverted().mapVector(QVector3D(1,0,0));
    right.setY(0);
    QVector3D up = QVector3D(0,1,0);

    if(key == Qt::Key_W) {
        cam->camTranslation += forward;
    }
    if(key == Qt::Key_A) {
        cam->camTranslation += right;
    }
    if(key == Qt::Key_S) {
        cam->camTranslation -= forward;
    }
    if(key == Qt::Key_D) {
        cam->camTranslation -= right;
    }
    if(key == Qt::Key_Q) {

        cam->camRotation.rotate(-10, up);
    }
    if(key == Qt::Key_E) {
        cam->camRotation.rotate(10, up);
    }
    if(key == Qt::Key_Control) {
        cam->camTranslation += up;
    }
    if(key == Qt::Key_Shift) {
        cam->camTranslation -= up;
    }


    update();
}

QPointF MainOpenGLWidget::pixelPosToViewPos(const QPointF &point) {

    return QPointF(2.0 * float(point.x() / width)  - 1.0, 2.0 * float(point.y() / height) - 1.0);
}

void MainOpenGLWidget::resetCamera() {

    //restore initial view matrix and update screen
    CameraLightInfo *cam = scene.getCameraLightInfo();

    cam->camRotation.setToIdentity();
    cam->camTranslation = QVector3D(0,0,0);
    update();
    cameraUpdated(cam->viewMatrix());
}

void MainOpenGLWidget::setShininess(int s) {
    shininess = s;
    update();
}

void MainOpenGLWidget::setTesselation(int t) {
    renderer.setTesselation(t);
    update();
}

void MainOpenGLWidget::setWireframe() {
    isWireframe = true;
    update();
}

void MainOpenGLWidget::setLightPos(QVector3D v) {
    scene.getCameraLightInfo()->lightPos = v;
    update();
}

void MainOpenGLWidget::setLightCol(QVector3D color) {
    scene.getCameraLightInfo()->lightCol = color;
    update();
}

void MainOpenGLWidget::setLightIntensity(int i) {
    scene.getCameraLightInfo()->lightInt = (float)i/10.0;
    update();
}

void MainOpenGLWidget::setPhong() {

    isWireframe = false;
    renderer.setRenderMode(SceneRenderer::Phong);
    update();
}

void MainOpenGLWidget::setHeightScaling(int scaling) {

    scene.setHeightScaling(scaling);
    update();
}

void MainOpenGLWidget::setDeferredPhong() {
    renderer.setRenderMode(SceneRenderer::Deferred);
    update();
}

void MainOpenGLWidget::setDeferredNormal() {
    renderer.setRenderMode(SceneRenderer::Normal);
    update();
}

void MainOpenGLWidget::setDeferredUV() {
    renderer.setRenderMode(SceneRenderer::UV);
    update();
}

void MainOpenGLWidget::setDeferredMaterial() {
    renderer.setRenderMode(SceneRenderer::Material);
    update();
}
void MainOpenGLWidget::setDeferredViewPos() {
    renderer.setRenderMode(SceneRenderer::ViewSpacePosition);
    update();
}

void MainOpenGLWidget::playAnimation() {
    scene.setAnimationPlay(true);
}

void MainOpenGLWidget::pauseAnimation() {
    scene.setAnimationPlay(false);
}

QChartView* MainOpenGLWidget::getChartView() {
    return renderer.getLogger()->getChartView();
}

void MainOpenGLWidget::loadModel(tinygltf::Model* gltf_model) {


    makeCurrent();
    scene.clear();
    scene.loadFromGLTF(*gltf_model);
    doneCurrent();

    update();


}

void MainOpenGLWidget::loadTerrain(QFile *pgmFile) {

    makeCurrent();
    scene.loadTerrain(pgmFile);
    doneCurrent();
}

