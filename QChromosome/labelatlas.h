#ifndef LABELATLAS_H
#define LABELATLAS_H


#include <QOpenGLFramebufferObject>

class LabelAtlas
{
public:
    explicit LabelAtlas();
    ~LabelAtlas();

    void initializeGL();
    GLuint texture() const;

    QRect addLabel(const QString& text);

private:
    QOpenGLFramebufferObjectFormat format;
    QOpenGLFramebufferObject *fbo;

    int width;
};

#endif // LABELATLAS_H
