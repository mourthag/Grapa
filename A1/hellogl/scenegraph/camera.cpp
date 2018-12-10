#include "camera.h"

Camera::Camera()
{

}

void Camera::rotate(QMatrix4x4 rotation) {
    camRotation *= rotation;
    createFrustumPlanes();
}

void Camera::translate(QVector3D vector)
{
    camTranslation += vector;
    createFrustumPlanes();
}

void Camera::resetCamera() {
    camTranslation = QVector3D(0,0,0);
    camRotation = QMatrix4x4();
    createFrustumPlanes();
}

QVector3D Camera::position() {
    return camTranslation;
}


QMatrix4x4 Camera::rotation() {
    return camRotation;
}

void Camera::setProjectionMatrix(QMatrix4x4 pMatrix) {
    projMatrix = pMatrix;
    createFrustumPlanes();
}

void Camera::setPosition(QVector3D position) {
    camTranslation = position;
    createFrustumPlanes();
}

QVector4D Camera::createPlane(QMatrix4x4 invMVP, QVector3D pointA, QVector3D pointB, QVector3D pointC)
{
    QVector3D mappedPointA = invMVP.map(pointA);
    QVector3D mappedPointB = invMVP.map(pointB);
    QVector3D mappedPointC = invMVP.map(pointC);
    QVector3D normal = QVector3D::crossProduct(mappedPointB - mappedPointA, mappedPointC - mappedPointA).normalized();
    float d = -QVector3D::dotProduct(mappedPointA, normal);
    QVector4D plane(normal, d);

    return plane;
}

void Camera::createFrustumPlanes() {

    frustumPlanes.clear();

    QVector3D pointA;
    QVector3D pointB;
    QVector3D pointC;

    QMatrix4x4 invMVP = ( projMatrix * viewMatrix()).inverted();

    //rightPlane
    pointA = QVector3D(1,-1,-1);
    pointB = QVector3D(1, 1,-1);
    pointC = QVector3D(1,-1, 1);
    frustumPlanes.push_back(createPlane(invMVP, pointA, pointB, pointC));

    //left
    pointA = QVector3D(-1, 1,-1);
    pointB = QVector3D(-1,-1,-1);
    pointC = QVector3D(-1, 1, 1);
    frustumPlanes.push_back(createPlane(invMVP, pointA, pointB, pointC));

    //top
    pointA = QVector3D(1, 1,-1);
    pointB = QVector3D(-1,1,-1);
    pointC = QVector3D(1, 1, 1);
    frustumPlanes.push_back(createPlane(invMVP, pointA, pointB, pointC));

    //bottom
    pointA = QVector3D(-1,-1,-1);
    pointB = QVector3D(1, -1,-1);
    pointC = QVector3D(-1,-1, 1);
    frustumPlanes.push_back(createPlane(invMVP, pointA, pointB, pointC));

    //near
    pointA = QVector3D(-1,-1,-1);
    pointB = QVector3D(-1, 1,-1);
    pointC = QVector3D(1,-1, -1);
    frustumPlanes.push_back(createPlane(invMVP, pointA, pointB, pointC));

    //far
    pointA = QVector3D(-1,1,1);
    pointB = QVector3D(-1, -1,1);
    pointC = QVector3D(1,1, 1);
    frustumPlanes.push_back(createPlane(invMVP, pointA, pointB, pointC));

}
