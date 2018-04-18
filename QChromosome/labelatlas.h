#ifndef LABELATLAS_H
#define LABELATLAS_H


#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLFramebufferObject>

class LabelAtlas : protected QOpenGLFunctions_3_3_Core
{
public:
    explicit LabelAtlas();
    ~LabelAtlas();

    void initializeGL();
    void paintGL();

    QRect addLabel(const QString& text);

private:
    QOpenGLFramebufferObjectFormat format;
    QOpenGLFramebufferObject *fbo;

    int width;
};

#endif // LABELATLAS_H
