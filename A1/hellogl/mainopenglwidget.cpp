#include "mainopenglwidget.h"
#include <qmath.h>
#define TINYGLTF_NO_STB_IMAGE_WRITE

MainOpenGLWidget::MainOpenGLWidget(QWidget *parent) : QOpenGLWidget(parent)
{
    frameCounter = 0;

    //update at ~60FPS
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(update()));
    timer->start(16);

    useQueryB = false;
    performanceLogger = new PerformanceChart();

    queryObjectsA = std::vector<GLuint>(NumQueries);
    queryObjectsB = std::vector<GLuint>(NumQueries);
    queryResultsA = std::vector<GLuint64>(NumQueries);
    queryResultsB = std::vector<GLuint64>(NumQueries);

    v = QMatrix4x4();
    p = QMatrix4x4();

    v.setToIdentity();
    p.setToIdentity();
    v.lookAt(QVector3D(2, 2, 3), QVector3D(0,0,0), QVector3D(0,1,0));

    //set up data and shading
    isWireframe = false;
    modelLoaded = false;
    tesselation = 1;

    //set up lightning
    lightInt = 1.0;
    lightPos = QVector3D(-2.0,2.0,-1.0);

    activeProgram = &phongProgram;
}

MainOpenGLWidget::~MainOpenGLWidget() {
}


void MainOpenGLWidget::initializeGL() {

    initializeOpenGLFunctions();

    glClearColor(1,1,1,1);

    gouraudProgram = new QOpenGLShaderProgram();
    gouraudProgram->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shader/gouraudvertshader.vert");
    gouraudProgram->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shader/gouraudfragshader.frag");
    gouraudProgram->link();

    phongProgram = new QOpenGLShaderProgram();
    phongProgram->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shader/phongvertshader.vert");
    phongProgram->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shader/phongfragshader.frag");
    phongProgram->link();

    scene.initGL();
    renderer.initGL();

    renderer.setRenderMode(SceneRenderer::Phong);

    glGenQueries(NumQueries, queryObjectsA.data());
    glGenQueries(NumQueries, queryObjectsB.data());
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

    updateUniforms();

    if(useQueryB)
        glQueryCounter(queryObjectsB[0], GL_TIMESTAMP);
    else
        glQueryCounter(queryObjectsA[0], GL_TIMESTAMP);

    renderer.drawScene(&scene, &v);

    if(useQueryB)
        glQueryCounter(queryObjectsB[1], GL_TIMESTAMP);
    else
        glQueryCounter(queryObjectsA[1], GL_TIMESTAMP);

    for (auto i = 0 ; i < NumQueries ; i++ ) {
        if(useQueryB) {
            glGetQueryObjectui64v(queryObjectsA[i], GL_QUERY_RESULT , queryResultsA.data() + i);
        }
        else{
            glGetQueryObjectui64v(queryObjectsB[i], GL_QUERY_RESULT , queryResultsB.data() + i);
        }
    }
    if(frameCounter % 33 == 0) {

        if(useQueryB)
            performanceLogger->addData(frameCounter, queryResultsA[1] - queryResultsA[0]);
        else
            performanceLogger->addData(frameCounter, queryResultsB[1] - queryResultsB[0]);

    }

    useQueryB = !useQueryB;
    frameCounter++;

}

void MainOpenGLWidget::resizeGL(int w, int h) {
    //update viewport
    glViewport(0,0,w,h);

    //for trackball calculations
    width = w;
    height = h;

    //projection matrix
    p.setToIdentity();
    p.perspective(45.0, (float)w/(float)h, 0.1, 10000);
    cameraUpdated(&v);
}

void MainOpenGLWidget::mousePressEvent(QMouseEvent *event) {
    //Drag start
    if(event->buttons() == Qt::RightButton || event->buttons() == Qt::LeftButton) {
        dragStart = event->pos();
    }

}

void MainOpenGLWidget::mouseMoveEvent(QMouseEvent *event) {

    QPoint p = event->pos();

    //Translate
    if(event->buttons() == Qt::RightButton) {


        QPointF drag = pixelPosToViewPos( p) - pixelPosToViewPos( dragStart);
        drag.setY(-drag.y());


        QVector3D dragVec = v.inverted().mapVector(QVector3D(drag));
        v.translate(dragVec);
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
        axis = v.inverted().mapVector(axis);
        axis.normalize();

        //create quaternion and rotate with it
        QQuaternion q = QQuaternion::fromAxisAndAngle(axis, angle);
        v.rotate(q);
    }

    dragStart = event->pos();
    //update screen and camera/statusbar
    update();
    cameraUpdated(&v);
}

void MainOpenGLWidget::wheelEvent(QWheelEvent *event) {
    //slow zooming down, cause delta is in degrees
    float val = event->delta() / 360.0;

    //calculate view direction
    QVector3D currentPos = v.inverted().map(QVector3D(0,0,0));
    QVector3D viewDir = currentPos.normalized();

    //translate along viewdir
    v.translate(val*viewDir);

    //update screen etc
    update();
    cameraUpdated(&v);
}

QPointF MainOpenGLWidget::pixelPosToViewPos(const QPointF &point) {

    return QPointF(2.0 * float(point.x() / width)  - 1.0, 2.0 * float(point.y() / height) - 1.0);
}

void MainOpenGLWidget::resetCamera() {

    //restore initial view matrix and update screen
    v.setToIdentity();
    v.lookAt(QVector3D(2,2,3), QVector3D(0,0,0), QVector3D(0,1,0));
    update();
    cameraUpdated(&v);
}

void MainOpenGLWidget::setShininess(int s) {
    shininess = s;
    update();
}

void MainOpenGLWidget::setTesselation(int t) {
    tesselation = t;

    update();
}

void MainOpenGLWidget::setWireframe() {
    isWireframe = true;
    update();
}

void MainOpenGLWidget::setLightPos(QVector3D v) {
    lightPos = v;
    update();
}

void MainOpenGLWidget::setLightIntensity(int i) {
    lightInt = (float)i/10.0;
    update();
}

void MainOpenGLWidget::setPhong() {

    isWireframe = false;
    activeProgram = &phongProgram;
    update();
}

void MainOpenGLWidget::setGouraud() {

    isWireframe = false;
    activeProgram = &gouraudProgram;
    update();
}

void MainOpenGLWidget::updateUniforms() {


    //update uniform values
    (*activeProgram)->bind();
    (*activeProgram)->setUniformValue("lightPos", lightPos);
    (*activeProgram)->setUniformValue("lightInt", lightInt);
    (*activeProgram)->setUniformValue("v", v);
    (*activeProgram)->setUniformValue("p", p);
}

QChartView* MainOpenGLWidget::getChartView() {
    return performanceLogger->getChartView();
}

void MainOpenGLWidget::loadModel(tinygltf::Model* gltf_model) {


    makeCurrent();
    scene.clear();
    scene.loadFromGLTF((*activeProgram),*gltf_model);
    doneCurrent();

    update();


}

