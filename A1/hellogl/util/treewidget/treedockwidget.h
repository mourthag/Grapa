#ifndef TREEDOCKWIDGET_H
#define TREEDOCKWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QSlider>
#include <QBoxLayout>
#include <QGroupBox>
#include <QSpinBox>
#include <QCheckBox>

#include "terrain/forrestdata.h"

class TreeDockWidget : public QWidget
{
    Q_OBJECT
public:
    explicit TreeDockWidget(QWidget *parent = nullptr);

signals:
    void dataChanged(ForrestData newData);
    void drawTerrainChanged(bool val);
    void drawTreesChanged(bool val);
    void drawSkyboxChanged(bool val);
    void frustumCullingChanged(bool val);
    void numImpostorsChanged(int val);
    void minSnowHeightChanged(float val);
    void snowGrowthRateChanged(float val);
    void minParticleChanged(int val);
    void maxParticleChanged(int val);

public slots:
    void changeNumTrees(int val);
    void changeMaxGeometryDistance(int val);
    void changeMaxImpostorDistance(int val);
    void changeDrawTerrain(bool val);
    void changeDrawTrees(bool val);
    void changeDrawSkybox(bool val);
    void changeFrustumCulling(bool val);
    void changeNumImpostors(int val);
    void changeMinSnowHeight(int val);
    void changeSnowGrowthRate(int val);
    void changeMinParticle(int val);
    void changeMaxParticle(int val);

private:
    QVBoxLayout *layout;

    QGroupBox *lodGroup;
    QGroupBox *treeGroup;
    QGroupBox *terrainGroup;
    QGroupBox *skyboxGroup;
    QGroupBox *frustumCullingGroup;
    QGroupBox *particleGroup;

    QSlider *numTreesSlider;
    QSlider *maxGeometryDistanceSlider;
    QSlider *maxImpostorDistanceSlider;
    QSlider *minSnowHeightSlider;
    QSlider *snowGrowthRateSlider;
    QSlider *minParticleSlider;
    QSlider *maxParticleSlider;

    QCheckBox *drawTerrainCheckBox;
    QCheckBox *drawSkyboxCheckBox;
    QCheckBox *drawTreesCheckBox;
    QCheckBox *frustumCullingCheckBox;

    QSpinBox *numImpostorSpinBox;

    ForrestData data;
    void createLODGroup();
    void createTreesGroup();
    void connectInterface();
    void createTerrainGroup();
    void createFrustumCullingGroup();
    void createSkyboxGroup();
    void createParticleGroup();
};

#endif // TREEDOCKWIDGET_H
