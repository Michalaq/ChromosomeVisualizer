#ifndef LABELATLAS_H
#define LABELATLAS_H


#include <QOpenGLFramebufferObject>
#include <QFont>

class LabelAtlas
{
public:
    explicit LabelAtlas();
    ~LabelAtlas();

    void initializeGL();
    GLuint texture() const;

    QRect addLabel(const QString& text);
    QRect size() const;

private:
    QOpenGLFramebufferObjectFormat format;
    QOpenGLFramebufferObject *fbo;

    int pos;
    int width;

    QFont font;
};

#endif // LABELATLAS_H
