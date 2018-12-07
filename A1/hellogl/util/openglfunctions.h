#ifndef OPENGLFUNCTIONS_H
#define OPENGLFUNCTIONS_H

#include <QOpenGLFunctions_4_3_Core>

class OpenGLFunctions : public QOpenGLFunctions_4_3_Core
{
public:
    static OpenGLFunctions* instance();
    static void initGL();
private:
    OpenGLFunctions();

    static OpenGLFunctions* _instance;
};

#endif // OPENGLFUNCTIONS_H
