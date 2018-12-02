#include "camera.h"

Camera::Camera()
{

}

void Camera::rotate(QMatrix4x4 rotation) {
    camRotation *= rotation;
}

void Camera::translate(QVector3D vector)
{
    camTranslation += vector;
}

void Camera::resetCamera() {
    camTranslation = QVector3D(0,0,0);
    camRotation = QMatrix4x4();
}

QVector3D Camera::position() {
    return camTranslation;
}


QMatrix4x4 Camera::rotation() {
    return camRotation;
}

void Camera::setProjectionMatrix(QMatrix4x4 pMatrix) {
    projMatrix = pMatrix;
}

void Camera::setPosition(QVector3D position) {
    camTranslation = position;
}
