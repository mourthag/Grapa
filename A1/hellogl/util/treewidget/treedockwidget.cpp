#include "treedockwidget.h"

void TreeDockWidget::createLODGroup()
{
    lodGroup = new QGroupBox(tr("Level of Detial"));

    QVBoxLayout *lodLayout = new QVBoxLayout;
    maxGeometryDistanceSlider = new QSlider(Qt::Horizontal);
    maxGeometryDistanceSlider->setRange(0, 10000);
    lodLayout->addWidget(maxGeometryDistanceSlider);
    maxImpostorDistanceSlider = new QSlider(Qt::Horizontal);
    maxImpostorDistanceSlider->setRange(0, 10000);
    lodLayout->addWidget(maxImpostorDistanceSlider);

    lodGroup->setLayout(lodLayout);
}

void TreeDockWidget::createTreesGroup()
{
    treeGroup = new QGroupBox(tr("Trees"));

    QVBoxLayout *treeLayout = new QVBoxLayout;
    numTreesSlider = new QSlider(Qt::Horizontal);
    numTreesSlider->setRange(1, 1000000);

    treeLayout->addWidget(numTreesSlider);

    treeGroup->setLayout(treeLayout);
}

void TreeDockWidget::connectInterface()
{
    connect(maxGeometryDistanceSlider, SIGNAL(valueChanged(int)), this, SLOT(maxGeometryDistanceChanged(int)));
    connect(maxImpostorDistanceSlider, SIGNAL(valueChanged(int)), this, SLOT(maxImpostorDistanceChanged(int)));
    connect(numTreesSlider, SIGNAL(valueChanged(int)), this, SLOT(numTreesChanged(int)));
}

TreeDockWidget::TreeDockWidget(QWidget *parent) : QWidget(parent)
{
    createTreesGroup();
    createLODGroup();

    layout = new QVBoxLayout;
    layout->addWidget(treeGroup);
    layout->addWidget(lodGroup);

    connectInterface();

    setLayout(layout);

}

void TreeDockWidget::maxGeometryDistanceChanged(int val) {
    data.maxGeometryDistance = val;
    dataChanged(data);
}

void TreeDockWidget::maxImpostorDistanceChanged(int val) {
    data.maxImpostorDistance = val;
    dataChanged(data);
}

void TreeDockWidget::numTreesChanged(int val) {
    data.numTrees = val;
    dataChanged(data);
}
