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

    QMenu *helpMenu = menuBar->addMenu("&Help");
    QAction *showAboutAction = helpMenu->addAction("&About");
    connect(showAboutAction, SIGNAL(triggered()), this, SLOT(showAboutBox()));

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

    setMenuBar(menuBar);

    QToolBar *toolBar = new QToolBar();
    QAction *resetCameraAction = toolBar->addAction("&Reset Camera");

    QSlider *shininessSlider = new QSlider();
    shininessSlider->setMaximum(10);
    shininessSlider->setOrientation(Qt::Horizontal);
    toolBar->addWidget(shininessSlider);

    tesselationSlider = new QSlider();
    tesselationSlider->setMinimum(1);
    tesselationSlider->setOrientation(Qt::Horizontal);
    toolBar->addWidget(tesselationSlider);

    toolBar->addAction(wireframeAction);
    toolBar->addAction(gouraudAction);
    toolBar->addAction(phongAction);

    addToolBar(toolBar);

    statusBar = new QStatusBar();
    setStatusBar(statusBar);

    widget = new MainOpenGLWidget;
    widget->setFormat(QSurfaceFormat::defaultFormat());
    setCentralWidget(widget);

    connect(resetCameraAction, SIGNAL(triggered()), widget, SLOT(resetCamera()));
    connect(widget, SIGNAL(cameraUpdated()), this, SLOT(updateStatusBar()));

    connect(shininessSlider, SIGNAL(valueChanged(int)), widget, SLOT(setShininess(int)));
    connect(tesselationSlider, SIGNAL(valueChanged(int)), widget, SLOT(setTesselation(int)));

    connect(wireframeAction, SIGNAL(triggered(bool)), widget, SLOT(setWireframe()));
    connect(gouraudAction, SIGNAL(triggered(bool)), widget, SLOT(setGouraud()));
    connect(phongAction, SIGNAL(triggered(bool)), widget, SLOT(setPhong()));

    shininessSlider->setValue(2);
    widget->resetCamera();
}

MainWindow::~MainWindow()
{

}

void MainWindow::showAboutBox()
{
    char* int_string;
    sprintf(int_string, "%d", widget->format().majorVersion());
    char str[64] = "Written by \n         Julian Spittel \n OpenGL-Version: \n        ";
    strcat(str, int_string);
    strcat(str, ".");
    sprintf(int_string, "%d", widget->format().minorVersion());
    strcat(str, int_string);
    QMessageBox::about(this, "About Hello GL", str);
}

void MainWindow::updateStatusBar() {
    QVector3D position = widget->getViewMat().inverted().map(QVector3D(0,0,0));
    QVector3D viewDir = position.normalized();
    std::stringstream message;
    message  << position.x() << " " << position.y() << " " << position.z() << "                 " << viewDir.x() << " " << viewDir.y() << " " << viewDir.z();



    statusBar->showMessage(QString(message.str().c_str()), 0);
}

void MainWindow::loadModel() {
    QString fileName = QFileDialog::getOpenFileName(this,
            tr("Open Model"), "",
            tr("Models (*.gltf);;All Files (*)"));
    std::string err;
    std::string warn;
    TinyGLTF loader;
    Model model;

    loader.LoadASCIIFromFile(&model, &err, &warn, fileName.toStdString());
    if(!err.empty()) {
        printf("Err: %s\n", err.c_str());
    }
    tesselationSlider->setDisabled(true);
    widget->loadModel(&model);
}
