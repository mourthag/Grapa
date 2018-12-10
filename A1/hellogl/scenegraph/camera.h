#ifndef CAMERA_H
#define CAMERA_H


#include <QMatrix4x4>

class Camera
{
public:
    Camera();

    QMatrix4x4 viewMatrix() {
        QMatrix4x4 translation;
        translation.translate(-camTranslation);
        return camRotation * translation;
    }
    QMatrix4x4 projectionMatrix() {
        return projMatrix;
    }

    void resetCamera();
    void translate(QVector3D vector);
    void rotate(QMatrix4x4 rotation);
    void setPosition(QVector3D position);
    void setProjectionMatrix(QMatrix4x4 pMatrix);

    QVector3D position();
    QMatrix4x4 rotation();

    std::vector<QVector4D> frustumPlanes;

private:
    QVector3D camTranslation;
    QMatrix4x4 camRotation;
    QMatrix4x4 projMatrix;
    void createFrustumPlanes();
    QVector4D createPlane(QMatrix4x4 invMVP, QVector3D pointA, QVector3D pointB, QVector3D pointC);
};

#endif // CAMERA_H
