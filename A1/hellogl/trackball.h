#ifndef TRACKBALL_H
#define TRACKBALL_H

#include <QQuaternion>

class TrackBall
{
public:
    TrackBall();

private:
    QQuaternion rot;
};

#endif // TRACKBALL_H
