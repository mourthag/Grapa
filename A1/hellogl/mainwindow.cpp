#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("Hello GL");

    QMenuBar *menuBar = new QMenuBar();

    QMenu *fileMenu = menuBar->addMenu("&File");
    QAction *exitAction = fileMenu->addAction("E&xit");
    exitAction->setShortcut(QKeySequence("Ctrl+q"));
    connect(exitAction, SIGNAL(triggered()), this, SLOT(close()));

    QMenu *helpMenu = menuBar->addMenu("&Help");
    QAction *showAboutAction = helpMenu->addAction("&About");
    connect(showAboutAction, SIGNAL(triggered()), this, SLOT(showAboutBox()));

    setMenuBar(menuBar);

    QToolBar *toolBar = new QToolBar();
    addToolBar(toolBar);

    QStatusBar *statusBar = new QStatusBar();
    setStatusBar(statusBar);


    widget = new MainOpenGLWidget;
    widget->setFormat(QSurfaceFormat::defaultFormat());
    setCentralWidget(widget);

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
