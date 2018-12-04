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

}

MainOpenGLWidget::~MainOpenGLWidget() {
}


void MainOpenGLWidget::initializeGL() {

    initializeOpenGLFunctions();


    makeCurrent();
    scene.initGL();
    terrainScene.initGL();
    makeCurrent();
    renderer.initGL();
    terrainRenderer.initGL();
    doneCurrent();
    terrainRenderer.setRenderMode(SceneRenderer::Phong);
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

    terrainRenderer.drawScene(&terrainScene);
    //renderer.drawScene(&scene);

}

void MainOpenGLWidget::resizeGL(int w, int h) {
    //update viewport
    glViewport(0,0,w,h);

    makeCurrent();
    renderer.updateFramebuffeSize(w, h);

    doneCurrent();

    Camera *cam = scene.getCamera();

    //for trackball calculations
    width = w;
    height = h;

    //projection matrix
    QMatrix4x4 projMatrix;
    projMatrix.perspective(45.0, (float)w/(float)h, 0.1, 10000);
    cam->setProjectionMatrix(projMatrix);
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

    Camera *cam = scene.getCamera();
    QMatrix4x4 camRotation = cam->rotation();

    //Translate
    if(event->buttons() == Qt::RightButton) {


        QPointF drag = pixelPosToViewPos( p) - pixelPosToViewPos( dragStart);
        drag.setY(-drag.y());


        QVector3D dragVec = camRotation.inverted().mapVector(QVector3D(drag));
        cam->translate(dragVec);
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
        axis = camRotation.inverted().mapVector(axis);
        axis.normalize();

        //create quaternion and rotate with it
        QQuaternion q = QQuaternion::fromAxisAndAngle(axis, angle);

        QMatrix4x4 rotation;
        rotation.rotate(q);
        cam->rotate(rotation);
    }

    dragStart = event->pos();
    //update screen and camera/statusbar
    update();
    cameraUpdated(cam->viewMatrix());
}

void MainOpenGLWidget::wheelEvent(QWheelEvent *event) {
    //slow zooming down, cause delta is in degrees
    float val = event->delta() / 360.0;

    Camera *cam = scene.getCamera();

    //calculate view direction
    QVector3D currentPos = cam->position();
    QVector3D viewDir = -currentPos.normalized();

    //translate along viewdir
    cam->translate(val * viewDir);

    //update screen etc
    update();
    cameraUpdated(cam->viewMatrix());
}

void MainOpenGLWidget::keyPressEvent(QKeyEvent *event) {
    //QOpenGLWidget::keyPressEvent(event);
    int key = event->key();

    Camera *cam = scene.getCamera();
    QMatrix4x4 cameraRotation = cam->rotation();

    QVector3D forward = cameraRotation.inverted().mapVector(QVector3D(0,0,1));
    forward.setY(0);
    forward.normalize();
    QVector3D right = cameraRotation.inverted().mapVector(QVector3D(1,0,0));
    right.setY(0);
    right.normalize();
    QVector3D up = QVector3D(0,1,0);

    if(key == Qt::Key_W) {
        cam->translate(-forward);
    }
    if(key == Qt::Key_A) {
        cam->translate(-right);
    }
    if(key == Qt::Key_S) {
        cam->translate(forward);
    }
    if(key == Qt::Key_D) {
        cam->translate(right);
    }
    if(key == Qt::Key_Q) {

        QMatrix4x4 rotation;
        rotation.rotate(-10, up);
        cam->rotate(rotation);
    }
    if(key == Qt::Key_E) {
        QMatrix4x4 rotation;
        rotation.rotate(10, up);
        cam->rotate(rotation);
    }
    if(key == Qt::Key_Control) {
        cam->translate(up);
    }
    if(key == Qt::Key_Shift) {
        cam->translate(-up);
    }


    update();
}

QPointF MainOpenGLWidget::pixelPosToViewPos(const QPointF &point) {

    return QPointF(2.0 * float(point.x() / width)  - 1.0, 2.0 * float(point.y() / height) - 1.0);
}

void MainOpenGLWidget::resetCamera() {

    //restore initial view matrix and update screen
    Camera *cam = scene.getCamera();

    cam->resetCamera();
    cam->setPosition(QVector3D(2,2,2));
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

    renderer.getLight()->lightPos = v;
    update();
}

void MainOpenGLWidget::setLightCol(QVector3D color) {

    renderer.getLight()->lightCol = color;
    update();
}

void MainOpenGLWidget::setLightIntensity(int i) {

    renderer.getLight()->lightInt = i;
    update();
}

void MainOpenGLWidget::setPhong() {

    isWireframe = false;
    renderer.setRenderMode(SceneRenderer::Phong);
    update();
}

void MainOpenGLWidget::setHeightScaling(int scaling) {

    terrainScene.setHeightScaling(scaling);
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
    terrainScene.loadTree(*gltf_model);
    terrainScene.setUpForrest();
    doneCurrent();

    update();


}

void MainOpenGLWidget::loadTerrain(QFile *pgmFile) {

    makeCurrent();
    terrainScene.loadTerrain(pgmFile);
    doneCurrent();
}

