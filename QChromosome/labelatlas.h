#ifndef LABELATLAS_H
#define LABELATLAS_H


#include <QOpenGLFramebufferObject>

class LabelAtlas
{
public:
    LabelAtlas();

    void bind();

private:
    QOpenGLFramebufferObject *fbo;
};

#endif // LABELATLAS_H
