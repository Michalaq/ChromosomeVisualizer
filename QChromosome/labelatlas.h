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

    QRect addLabel(const QString& text, const QFont &font);
    QRect size() const;

private:
    QOpenGLFramebufferObjectFormat format;
    QOpenGLFramebufferObject *fbo;

    int pos;
    int width;
};

#endif // LABELATLAS_H
