#-------------------------------------------------
#
# Project created by QtCreator 2018-10-18T17:30:45
#
#-------------------------------------------------

QT       += core gui opengl charts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = hellogl
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
    terrain/terrain.cpp \
    util/performancechart.cpp \
    scenegraph/animation.cpp \
    scenegraph/node.cpp \
    scenegraph/openglmodel.cpp \
    scenegraph/scene.cpp \
    scenegraph/scenerenderer.cpp \
    main/main.cpp \
    main/mainopenglwidget.cpp \
    main/mainwindow.cpp \
    terrain/forrest.cpp \
    terrain/tree.cpp \
    terrain/terrainscene.cpp \
    scenegraph/camera.cpp \
    terrain/terrainscenerenderer.cpp \
    util/treewidget/treedockwidget.cpp \
    util/openglfunctions.cpp

HEADERS += \
    json.hpp \
    tiny_gltf.h \
    stb_image.h \
    terrain/terrain.h \
    util/performancechart.h \
    scenegraph/animation.h \
    scenegraph/node.h \
    scenegraph/openglmodel.h \
    scenegraph/scene.h \
    scenegraph/scenerenderer.h \
    main/mainopenglwidget.h \
    main/mainwindow.h \
    terrain/forrest.h \
    terrain/tree.h \
    terrain/terrainscene.h \
    scenegraph/camera.h \
    scenegraph/material.h \
    scenegraph/light.h \
    terrain/terrainscenerenderer.h \
    util/treewidget/treedockwidget.h \
    terrain/forrestdata.h \
    util/openglfunctions.h

DISTFILES += \
    shader/default/phongfragshader.frag \
    shader/default/phongvertshader.vert \
    shader/default/gouraudvertshader.vert \
    shader/default/gouraudfragshader.frag \
    shader/deferred/geometrypassfragshader.frag \
    shader/deferred/geometrypassvertshader.vert \
    shader/deferred/lightingpassvertshader.vert \
    shader/deferred/lightingpassfragshader.frag \
    shader/deferred/lightingpasspointsvertshader.vert \
    shader/deferred/lightingpasspointsfragshader.frag \
    shader/terrain/terrainvertshader.vert \
    shader/terrain/terrainfragshader.frag \
    shader/terrain/terraintesselationcontrolshader.tcs \
    shader/terrain/terraintesselationevalshader.tes \
    shader/terrain/treefragshader.frag \
    shader/terrain/treevertshader.vert \
    shader/terrain/treecompshader.comp \
    shader/terrain/treeimpostorfragshader.frag \
    shader/terrain/treeimpostorvertshader.vert

RESOURCES += \
    hellogl.qrc

STATECHARTS +=
