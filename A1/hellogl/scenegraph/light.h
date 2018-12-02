#ifndef LIGHT_H
#define LIGHT_H

#include <QVector3D>

struct Light {
    QVector3D lightPos;
    QVector3D lightCol;
    float lightInt;
};

#endif // LIGHT_H
