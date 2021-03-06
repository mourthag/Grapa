#include "treedockwidget.h"

void TreeDockWidget::createLODGroup()
{
    lodGroup = new QGroupBox(tr("Level of Detail"));

    QVBoxLayout *lodLayout = new QVBoxLayout;
    lodLayout->addWidget(new QLabel(tr("Max distance full geometry")));

    maxGeometryDistanceSlider = new QSlider(Qt::Horizontal);
    maxGeometryDistanceSlider->setRange(0, 10000);
    lodLayout->addWidget(maxGeometryDistanceSlider);

    lodLayout->addWidget(new QLabel(tr("Max distance impostor")));

    maxImpostorDistanceSlider = new QSlider(Qt::Horizontal);
    maxImpostorDistanceSlider->setRange(0, 10000);
    lodLayout->addWidget(maxImpostorDistanceSlider);

    lodLayout->addWidget(new QLabel(tr("Number of impostors")));

    numImpostorSpinBox = new QSpinBox();
    numImpostorSpinBox->setRange(1, 120);
    lodLayout->addWidget(numImpostorSpinBox);

    lodGroup->setLayout(lodLayout);
}

void TreeDockWidget::createTreesGroup()
{
    treeGroup = new QGroupBox(tr("Trees"));

    QVBoxLayout *treeLayout = new QVBoxLayout;

    drawTreesCheckBox = new QCheckBox(tr("Draw trees"));
    drawTreesCheckBox->setChecked(true);
    treeLayout->addWidget(drawTreesCheckBox);

    treeLayout->addWidget(new QLabel(tr("Number of trees")));

    numTreesSlider = new QSlider(Qt::Horizontal);
    numTreesSlider->setRange(1, 1000000);

    treeLayout->addWidget(numTreesSlider);

    treeGroup->setLayout(treeLayout);
}

void TreeDockWidget::connectInterface()
{
    connect(maxGeometryDistanceSlider, SIGNAL(valueChanged(int)), this, SLOT(changeMaxGeometryDistance(int)));
    connect(maxImpostorDistanceSlider, SIGNAL(valueChanged(int)), this, SLOT(changeMaxImpostorDistance(int)));
    connect(numImpostorSpinBox, SIGNAL(valueChanged(int)), this, SLOT(changeNumImpostors(int)));
    connect(numTreesSlider, SIGNAL(valueChanged(int)), this, SLOT(changeNumTrees(int)));
    connect(drawTerrainCheckBox, SIGNAL(toggled(bool)), this, SLOT(changeDrawTerrain(bool)));
    connect(drawTreesCheckBox, SIGNAL(toggled(bool)), this, SLOT(changeDrawTrees(bool)));
    connect(drawSkyboxCheckBox, SIGNAL(toggled(bool)), this, SLOT(changeDrawSkybox(bool)));
    connect(frustumCullingCheckBox, SIGNAL(toggled(bool)), this, SLOT(changeFrustumCulling(bool)));
    connect(minSnowHeightSlider, SIGNAL(valueChanged(int)), this, SLOT(changeMinSnowHeight(int)));
    connect(minParticleSlider, SIGNAL(valueChanged(int)), this, SLOT(changeMinParticle(int)));
    connect(maxParticleSlider, SIGNAL(valueChanged(int)), this, SLOT(changeMaxParticle(int)));
    connect(snowGrowthRateSlider, SIGNAL(valueChanged(int)), this, SLOT(changeSnowGrowthRate(int)));
}

void TreeDockWidget::createTerrainGroup()
{
    terrainGroup = new QGroupBox(tr("Terrain"));
    QVBoxLayout *terrainLayout = new QVBoxLayout;

    drawTerrainCheckBox = new QCheckBox(tr("Draw terrain"));
    drawTerrainCheckBox->setChecked(true);
    terrainLayout->addWidget(drawTerrainCheckBox);
    terrainLayout->addWidget(new QLabel(tr("Snow starting height")));

    minSnowHeightSlider = new QSlider(Qt::Horizontal);
    minSnowHeightSlider->setRange(0,1000);
    minSnowHeightSlider->setValue(200);
    terrainLayout->addWidget(minSnowHeightSlider);

    terrainLayout->addWidget(new QLabel(tr("Snow increase rate")));

    snowGrowthRateSlider = new QSlider(Qt::Horizontal);
    snowGrowthRateSlider->setRange(0,100);
    snowGrowthRateSlider->setValue(4);
    terrainLayout->addWidget(snowGrowthRateSlider);

    terrainGroup->setLayout(terrainLayout);
}

void TreeDockWidget::createFrustumCullingGroup()
{
    frustumCullingGroup = new QGroupBox(tr("Frustum culling"));

    QVBoxLayout *frustumCullingLayout = new QVBoxLayout;

    frustumCullingCheckBox = new QCheckBox(tr("Frustum culling"));
    frustumCullingCheckBox->setChecked(true);
    frustumCullingLayout->addWidget(frustumCullingCheckBox);

    frustumCullingGroup->setLayout(frustumCullingLayout);
}

void TreeDockWidget::createSkyboxGroup()
{
    skyboxGroup = new QGroupBox(tr("Skybox"));

    QVBoxLayout *skyboxLayout = new QVBoxLayout;

    drawSkyboxCheckBox = new QCheckBox(tr("Draw skybox"));
    drawSkyboxCheckBox->setChecked(true);
    skyboxLayout->addWidget(drawSkyboxCheckBox);

    skyboxGroup->setLayout(skyboxLayout);
}

void TreeDockWidget::createParticleGroup()
{
    particleGroup = new QGroupBox(tr("Snow Particles"));

    QVBoxLayout *particleBoxLayout = new QVBoxLayout;

    particleBoxLayout->addWidget(new QLabel(tr("Minimum number of particles")));

    minParticleSlider = new QSlider(Qt::Horizontal);
    minParticleSlider->setRange(0, 1000000);
    minParticleSlider->setValue(100);
    particleBoxLayout->addWidget(minParticleSlider);

    particleBoxLayout->addWidget(new QLabel(tr("Maximum number of particles")));

    maxParticleSlider = new QSlider(Qt::Horizontal);
    maxParticleSlider->setRange(1, 1000000);
    maxParticleSlider->setValue(5000);
    particleBoxLayout->addWidget(maxParticleSlider);

    particleGroup->setLayout(particleBoxLayout);
}

TreeDockWidget::TreeDockWidget(QWidget *parent) : QWidget(parent)
{

    createTerrainGroup();
    createTreesGroup();
    createLODGroup();
    createFrustumCullingGroup();
    createSkyboxGroup();
    createParticleGroup();

    layout = new QVBoxLayout;
    layout->addWidget(terrainGroup);
    layout->addWidget(particleGroup);
    layout->addWidget(treeGroup);
    layout->addWidget(lodGroup);
    layout->addWidget(frustumCullingGroup);
    layout->addWidget(skyboxGroup);

    connectInterface();

    setLayout(layout);

}

void TreeDockWidget::changeMaxGeometryDistance(int val) {
    data.maxGeometryDistance = val;
    dataChanged(data);
}

void TreeDockWidget::changeMaxImpostorDistance(int val) {
    data.maxImpostorDistance = val;
    dataChanged(data);
}

void TreeDockWidget::changeNumTrees(int val) {
    data.numTrees = val;
    dataChanged(data);
}

void TreeDockWidget::changeDrawTerrain(bool val) {
    drawTerrainChanged(val);
}

void TreeDockWidget::changeDrawTrees(bool val) {
    drawTreesChanged(val);
}

void TreeDockWidget::changeDrawSkybox(bool val) {
    drawSkyboxChanged(val);
}

void TreeDockWidget::changeFrustumCulling(bool val) {
    frustumCullingChanged(val);
}

void TreeDockWidget::changeNumImpostors(int val) {
    numImpostorsChanged(val);
}

void TreeDockWidget::changeMinSnowHeight(int val) {
    minSnowHeightChanged((float)val/4.0);
}

void TreeDockWidget::changeSnowGrowthRate(int val) {
    snowGrowthRateChanged((float)val/40.0);
}

void TreeDockWidget::changeMinParticle(int val) {
    minParticleChanged(val);
}

void TreeDockWidget::changeMaxParticle(int val) {
    maxParticleChanged(val);
}
