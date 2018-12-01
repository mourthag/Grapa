#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets>

#include "mainopenglwidget.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void showAboutBox();
    void updateStatusBar(QMatrix4x4 viewMat);
    void loadModel();
    void loadTerrain();
    void updateLightPos();
    void updateLightCol();

private:

    QStatusBar *statusBar;

    QLineEdit *lightPosXInput;
    QLineEdit *lightPosYInput;
    QLineEdit *lightPosZInput;
    QLineEdit *lightColRInput;
    QLineEdit *lightColGInput;
    QLineEdit *lightColBInput;

    QSlider *tesselationSlider;

    MainOpenGLWidget *widget;
};

#endif // MAINWINDOW_H
