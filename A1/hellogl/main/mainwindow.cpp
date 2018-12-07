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
    setMinimumSize(600,600);

    QMenuBar *menuBar = new QMenuBar();

    QMenu *fileMenu = menuBar->addMenu("&File");

    QAction *openModelAction = fileMenu->addAction("&Open model...");
    connect(openModelAction, SIGNAL(triggered(bool)), this, SLOT(loadModel()));

    QAction *openTerrainAction = fileMenu->addAction("Open Terrain...");
    connect(openTerrainAction, SIGNAL(triggered(bool)), this, SLOT(loadTerrain()));

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

    QAction *phongAction =shadingMenu->addAction("Display standard phong shading");
    phongAction->setShortcut(QKeySequence("Ctrl+3"));
    phongAction->setCheckable(true);
    phongAction->setChecked(true);
    phongAction->setIcon(QIcon(":/img/phong.png"));

    shadingOptions->setExclusive(true);
    shadingOptions->addAction(wireframeAction);
    shadingOptions->addAction(phongAction);

    QMenu *animationMenu = menuBar->addMenu("&Animation");
    QActionGroup *animationGroup = new QActionGroup(this);

    QAction *playAnimationAction = animationMenu->addAction("Play Animation");
    playAnimationAction->setCheckable(true);

    QAction *pauseAnimationAction = animationMenu->addAction("Pause Animation");
    pauseAnimationAction->setCheckable(true);

    animationGroup->setExclusive(true);
    animationGroup->addAction(playAnimationAction);
    animationGroup->addAction(pauseAnimationAction);
    playAnimationAction->setChecked(true);

    QMenu *visualizationMenu = menuBar->addMenu("&Visualisation");
    QActionGroup *visualizationGroup = new QActionGroup(this);

    QAction *checkStandardPhongAction = visualizationMenu->addAction("Display standard phong shading");
    checkStandardPhongAction->setCheckable(true);
    checkStandardPhongAction->setChecked(true);

    QAction *checkPhongAction = visualizationMenu->addAction("Display deferred &phong shading");
    checkPhongAction->setCheckable(true);

    QAction *checkNormalAction = visualizationMenu->addAction("Display &normals");
    checkNormalAction->setCheckable(true);

    QAction *checkUVAction = visualizationMenu->addAction("Display &UVs");
    checkUVAction->setCheckable(true);

    QAction *checkMaterialAction = visualizationMenu->addAction("Display &Materials");
    checkMaterialAction->setCheckable(true);

    QAction *checkViewPosAction = visualizationMenu->addAction("Display &viewspace position");
    checkViewPosAction->setCheckable(true);


    visualizationGroup->setExclusive(true);
    visualizationGroup->addAction(checkPhongAction);
    visualizationGroup->addAction(checkNormalAction);
    visualizationGroup->addAction(checkUVAction);
    visualizationGroup->addAction(checkMaterialAction);
    visualizationGroup->addAction(checkViewPosAction);
    visualizationGroup->addAction(checkStandardPhongAction);

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

    lightColRInput = new QLineEdit();
    lightColGInput = new QLineEdit();
    lightColBInput = new QLineEdit();
    lightColRInput->setValidator(posValidator);
    lightColRInput->setMaximumWidth(40);
    lightColRInput->setText("1.00");
    lightColGInput->setValidator(posValidator);
    lightColGInput->setMaximumWidth(40);
    lightColGInput->setText("1.00");
    lightColBInput->setValidator(posValidator);
    lightColBInput->setMaximumWidth(40);
    lightColBInput->setText("1.00");

    toolBar->addWidget(lightColRInput);
    toolBar->addWidget(lightColGInput);
    toolBar->addWidget(lightColBInput);

    //Slider for light intensity
    QSlider *lightIntSlider = new QSlider();
    lightIntSlider->setMaximum(1000);
    lightIntSlider->setValue(10);
    lightIntSlider->setOrientation(Qt::Horizontal);
    toolBar->addWidget(lightIntSlider);

    toolBar->addSeparator();

    //slider for phong exponent
    QSlider *terrainHeightSlider = new QSlider();
    terrainHeightSlider->setMinimum(0);
    terrainHeightSlider->setMaximum(1000);
    terrainHeightSlider->setOrientation(Qt::Horizontal);
    toolBar->addWidget(terrainHeightSlider);

    //slider for tesselation level
    tesselationSlider = new QSlider();
    tesselationSlider->setMinimum(1);
    tesselationSlider->setOrientation(Qt::Horizontal);
    toolBar->addWidget(tesselationSlider);

    //add shading options to toolbar here, layout...
    toolBar->addAction(wireframeAction);
    toolBar->addAction(phongAction);

    addToolBar(toolBar);

    statusBar = new QStatusBar();
    setStatusBar(statusBar);

    //create the opengl widget
    widget = new MainOpenGLWidget;
    widget->setFormat(QSurfaceFormat::defaultFormat());
    setCentralWidget(widget);

    treeDataWidget = new TreeDockWidget();
    QDockWidget *forrestDockWidget = new QDockWidget(tr("Forrest parameters"), this);
    forrestDockWidget->setWidget(treeDataWidget);
    addDockWidget(Qt::RightDockWidgetArea, forrestDockWidget);

    QDockWidget *performanceDockWidget = new QDockWidget(tr("Performance Log"), this);
    performanceDockWidget->setWidget(widget->getChartView());
    addDockWidget(Qt::BottomDockWidgetArea, performanceDockWidget);

    //connect all the widget slots and signals for the interface
    connect(resetCameraAction, SIGNAL(triggered()), widget, SLOT(resetCamera()));
    connect(widget, SIGNAL(cameraUpdated(QMatrix4x4)), this, SLOT(updateStatusBar(QMatrix4x4)));

    connect(lightPosXInput, SIGNAL(returnPressed()), this, SLOT(updateLightPos()));
    connect(lightPosYInput, SIGNAL(returnPressed()), this, SLOT(updateLightPos()));
    connect(lightPosZInput, SIGNAL(returnPressed()), this, SLOT(updateLightPos()));
    connect(lightColRInput, SIGNAL(returnPressed()), this, SLOT(updateLightCol()));
    connect(lightColGInput, SIGNAL(returnPressed()), this, SLOT(updateLightCol()));
    connect(lightColBInput, SIGNAL(returnPressed()), this, SLOT(updateLightCol()));
    connect(lightIntSlider, SIGNAL(valueChanged(int)), widget, SLOT(setLightIntensity(int)));

    connect(terrainHeightSlider, SIGNAL(valueChanged(int)), widget, SLOT(setHeightScaling(int)));
    connect(tesselationSlider, SIGNAL(valueChanged(int)), widget, SLOT(setTesselation(int)));

    connect(wireframeAction, SIGNAL(triggered(bool)), widget, SLOT(setWireframe()));
    connect(phongAction, SIGNAL(triggered(bool)), widget, SLOT(setPhong()));

    connect(checkStandardPhongAction , SIGNAL(triggered()), widget, SLOT(setPhong()));
    connect(checkPhongAction , SIGNAL(triggered()), widget, SLOT(setDeferredPhong()));
    connect(checkNormalAction, SIGNAL(triggered()), widget, SLOT(setDeferredNormal()));
    connect(checkUVAction, SIGNAL(triggered()), widget, SLOT(setDeferredUV()));
    connect(checkMaterialAction, SIGNAL(triggered()), widget, SLOT(setDeferredMaterial()));
    connect(checkViewPosAction, SIGNAL(triggered()), widget, SLOT(setDeferredViewPos()));

    connect(playAnimationAction, SIGNAL(triggered(bool)), widget, SLOT(playAnimation()));
    connect(pauseAnimationAction, SIGNAL(triggered(bool)), widget, SLOT(pauseAnimation()));

    connect(treeDataWidget, SIGNAL(dataChanged(ForrestData)), widget, SLOT(setForrestData(ForrestData)));

    widget->resetCamera();
    //adjust slider here to init correct value in widget
    terrainHeightSlider->setValue(2);
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

void MainWindow::updateStatusBar(QMatrix4x4 viewMat) {
    //calculate position by applying the inverse viewmatrix to a null vector
    QVector3D position = viewMat.inverted().map(QVector3D(0,0,0));
    //view dir is obviously the negative position but normalized
    QVector3D viewDir = viewMat.mapVector(QVector3D(0,0,1));

    //wrap it into a string
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

void MainWindow::updateLightCol() {
    QVector3D color = QVector3D(0,0,0);
    color.setX(lightColRInput->text().toFloat());
    color.setY(lightColGInput->text().toFloat());
    color.setZ(lightColBInput->text().toFloat());
    widget->setLightCol(color);
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
    widget->loadModel(&model);
}

void MainWindow::loadTerrain() {
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Terrain"), "",
                                                    tr("Models (*.pgm);;All files(*)"));
    if(fileName.isEmpty() || fileName.isNull())
        return;

    QFile pgmFile(fileName);
    //load terrain
    widget->loadTerrain(&pgmFile);
}
