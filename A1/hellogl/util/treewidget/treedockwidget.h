#ifndef TREEDOCKWIDGET_H
#define TREEDOCKWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QSlider>
#include <QBoxLayout>
#include <QGroupBox>

#include "terrain/forrestdata.h"

class TreeDockWidget : public QWidget
{
    Q_OBJECT
public:
    explicit TreeDockWidget(QWidget *parent = nullptr);

signals:
    void dataChanged(ForrestData newData);

public slots:
    void numTreesChanged(int val);
    void maxGeometryDistanceChanged(int val);
    void maxImpostorDistanceChanged(int val);

private:
    QVBoxLayout *layout;

    QGroupBox *lodGroup;
    QGroupBox *treeGroup;

    QSlider *numTreesSlider;
    QSlider *maxGeometryDistanceSlider;
    QSlider *maxImpostorDistanceSlider;

    ForrestData data;
    void createLODGroup();
    void createTreesGroup();
    void connectInterface();
};

#endif // TREEDOCKWIDGET_H
