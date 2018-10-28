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
    void updateStatusBar();
    void loadModel();
    void updateLightPos();

private:

    QStatusBar *statusBar;

    QLineEdit *lightPosXInput;
    QLineEdit *lightPosYInput;
    QLineEdit *lightPosZInput;

    QSlider *tesselationSlider;

    MainOpenGLWidget *widget;
};

#endif // MAINWINDOW_H
