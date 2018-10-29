#include "mainwindow.h"
#include <sstream>
#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define TINYGLTF_NO_STB_IMAGE_WRITE

#include "tiny_gltf.h"

using namespace tinygltf;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("Hello GL");

    QMenuBar *menuBar = new QMenuBar();

    QMenu *fileMenu = menuBar->addMenu("&File");

    QAction *openModelAction = fileMenu->addAction("&Open model...");
    connect(openModelAction, SIGNAL(triggered(bool)), this, SLOT(loadModel()));

    QAction *exitAction = fileMenu->addAction("E&xit");
    exitAction->setShortcut(QKeySequence("Ctrl+q"));
    connect(exitAction, SIGNAL(triggered()), this, SLOT(close()));

    //shading options and respective menu
    QMenu *shadingMenu = menuBar->addMenu("&Shading");
    QActionGroup *shadingOptions = new QActionGroup(this);

    QAction *wireframeAction = shadingMenu->addAction("None (Wireframe)");
    wireframeAction->setShortcut(QKeySequence("Ctrl+1"));
    wireframeAction->setCheckable(true);
    wireframeAction->setIcon(QIcon(":/img/wireframe.png"));

    QAction *gouraudAction = shadingMenu->addAction("Gouraud");
    gouraudAction->setShortcut(QKeySequence("Ctrl+2"));
    gouraudAction->setCheckable(true);
    gouraudAction->setIcon(QIcon(":/img/gouraud.png"));

    QAction *phongAction =shadingMenu->addAction("Phong");
    phongAction->setShortcut(QKeySequence("Ctrl+3"));
    phongAction->setCheckable(true);
    phongAction->setChecked(true);
    phongAction->setIcon(QIcon(":/img/phong.png"));

    shadingOptions->setExclusive(true);
    shadingOptions->addAction(wireframeAction);
    shadingOptions->addAction(gouraudAction);
    shadingOptions->addAction(phongAction);


    QMenu *helpMenu = menuBar->addMenu("&Help");
    QAction *showAboutAction = helpMenu->addAction("&About");
    connect(showAboutAction, SIGNAL(triggered()), this, SLOT(showAboutBox()));

    setMenuBar(menuBar);

    QToolBar *toolBar = new QToolBar();
    QAction *resetCameraAction = toolBar->addAction("&Reset Camera");

    toolBar->addSeparator();

    //3 Inputs for every axis of the light position
    lightPosXInput = new QLineEdit();
    lightPosYInput = new QLineEdit();
    lightPosZInput = new QLineEdit();
    QValidator *posValidator = new QDoubleValidator();
    lightPosXInput->setValidator(posValidator);
    lightPosXInput->setMaximumWidth(40);
    lightPosXInput->setText("-2.00");
    lightPosYInput->setValidator(posValidator);
    lightPosYInput->setMaximumWidth(40);
    lightPosYInput->setText("2.00");
    lightPosZInput->setValidator(posValidator);
    lightPosZInput->setMaximumWidth(40);
    lightPosZInput->setText("1.00");

    toolBar->addWidget(lightPosXInput);
    toolBar->addWidget(lightPosYInput);
    toolBar->addWidget(lightPosZInput);

    //Slider for light intensity
    QSlider *lightIntSlider = new QSlider();
    lightIntSlider->setMaximum(100);
    lightIntSlider->setValue(10);
    lightIntSlider->setOrientation(Qt::Horizontal);
    toolBar->addWidget(lightIntSlider);

    toolBar->addSeparator();

    //slider for phong exponent
    QSlider *shininessSlider = new QSlider();
    shininessSlider->setMaximum(20);
    shininessSlider->setOrientation(Qt::Horizontal);
    toolBar->addWidget(shininessSlider);

    //slider for tesselation level
    tesselationSlider = new QSlider();
    tesselationSlider->setMinimum(1);
    tesselationSlider->setOrientation(Qt::Horizontal);
    toolBar->addWidget(tesselationSlider);

    //add shading options to toolbar here, layout...
    toolBar->addAction(wireframeAction);
    toolBar->addAction(gouraudAction);
    toolBar->addAction(phongAction);

    addToolBar(toolBar);

    statusBar = new QStatusBar();
    setStatusBar(statusBar);

    //create the opengl widget
    widget = new MainOpenGLWidget;
    widget->setFormat(QSurfaceFormat::defaultFormat());
    setCentralWidget(widget);

    //connect all the widget slots and signals for the interface
    connect(resetCameraAction, SIGNAL(triggered()), widget, SLOT(resetCamera()));
    connect(widget, SIGNAL(cameraUpdated()), this, SLOT(updateStatusBar()));

    connect(lightPosXInput, SIGNAL(returnPressed()), this, SLOT(updateLightPos()));
    connect(lightPosYInput, SIGNAL(returnPressed()), this, SLOT(updateLightPos()));
    connect(lightPosZInput, SIGNAL(returnPressed()), this, SLOT(updateLightPos()));
    connect(lightIntSlider, SIGNAL(valueChanged(int)), widget, SLOT(setLightIntensity(int)));

    connect(shininessSlider, SIGNAL(valueChanged(int)), widget, SLOT(setShininess(int)));
    connect(tesselationSlider, SIGNAL(valueChanged(int)), widget, SLOT(setTesselation(int)));

    connect(wireframeAction, SIGNAL(triggered(bool)), widget, SLOT(setWireframe()));
    connect(gouraudAction, SIGNAL(triggered(bool)), widget, SLOT(setGouraud()));
    connect(phongAction, SIGNAL(triggered(bool)), widget, SLOT(setPhong()));

    widget->resetCamera();
    //adjust slider here to init correct value in widget
    shininessSlider->setValue(2);
}

MainWindow::~MainWindow()
{

}

void MainWindow::showAboutBox()
{
    std::stringstream message;
    message << "Written by: \n";
    message << "Julian Spittel \n";
    message << "OpenGL-Version: \n";
    message << widget->format().majorVersion() << "." << widget->format().minorVersion();

    QMessageBox::about(this, "About Hello GL", QString(message.str().c_str()));
}

void MainWindow::updateStatusBar() {
    QVector3D position = widget->getViewMat().inverted().map(QVector3D(0,0,0));
    QVector3D viewDir = position.normalized();
    std::stringstream message;
    message  << position.x() << " " << position.y() << " " << position.z() << "                 " << viewDir.x() << " " << viewDir.y() << " " << viewDir.z();



    statusBar->showMessage(QString(message.str().c_str()));
}

void MainWindow::updateLightPos() {
    QVector3D position = QVector3D(0,0,0);
    position.setX(lightPosXInput->text().toFloat());
    position.setY(lightPosYInput->text().toFloat());
    position.setZ(lightPosZInput->text().toFloat());
    widget->setLightPos(position);
}

void MainWindow::loadModel() {
    QString fileName = QFileDialog::getOpenFileName(this,
            tr("Open Model"), "",
            tr("Models (*.gltf);;All Files (*)"));

    //Cancelled
    if(fileName.isEmpty() || fileName.isNull()){
        return;
    }

    std::string err;
    std::string warn;
    TinyGLTF loader;
    Model model;

    loader.LoadASCIIFromFile(&model, &err, &warn, fileName.toStdString());
    if(!err.empty()) {
        qDebug() << err.c_str();
        return;
    }
    tesselationSlider->setDisabled(true);
    widget->loadModel(&model);
}
