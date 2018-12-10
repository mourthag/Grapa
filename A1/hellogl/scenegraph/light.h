#ifndef LIGHT_H
#define LIGHT_H

#include <QVector3D>

struct Light {
    QVector3D lightPos;
    QVector3D lightCol;
    float lightInt;

    Light() {
        lightPos = QVector3D(0,0,0);
        lightCol = QVector3D(1,1,1);
        lightInt = 1.0;
    }
};

#endif // LIGHT_H
