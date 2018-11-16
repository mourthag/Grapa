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
        main.cpp \
        mainwindow.cpp \
    mainopenglwidget.cpp \
    openglmodel.cpp \
    scene.cpp \
    node.cpp \
    animation.cpp \
    performancechart.cpp \
    scenerenderer.cpp \
    terrain.cpp

HEADERS += \
        mainwindow.h \
    mainopenglwidget.h \
    json.hpp \
    tiny_gltf.h \
    stb_image.h \
    openglmodel.h \
    scene.h \
    node.h \
    animation.h \
    performancechart.h \
    scenerenderer.h \
    terrain.h

DISTFILES += \
    shader/phongfragshader.frag \
    shader/phongvertshader.vert \
    shader/gouraudvertshader.vert \
    shader/gouraudfragshader.frag \
    shader/geometrypassfragshader.frag \
    shader/geometrypassvertshader.vert \
    shader/lightingpassvertshader.vert \
    shader/lightingpassfragshader.frag \
    shader/lightingpasspointsvertshader.vert \
    shader/lightingpasspointsfragshader.frag \
    shader/terrainvertshader.vert \
    shader/terrainfragshader.frag \
    shader/terraintesselationcontrolshader.tcs \
    shader/terraintesselationevalshader.tes

RESOURCES += \
    hellogl.qrc
