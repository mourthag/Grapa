#include "openglfunctions.h"

OpenGLFunctions *OpenGLFunctions::_instance = NULL;

OpenGLFunctions::OpenGLFunctions()
{

}

void OpenGLFunctions::initGL() {
    instance()->initializeOpenGLFunctions();
}

OpenGLFunctions* OpenGLFunctions::instance() {
    if(_instance == NULL) {
        _instance = new OpenGLFunctions;
    }
    return _instance;
}
