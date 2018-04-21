#ifndef LABELATLAS_H
#define LABELATLAS_H


#include <QOpenGLFramebufferObject>
#include <QFont>
#include <QFontMetrics>

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

    int pos;
    int size;

    QFont font;
    QFontMetrics fmetrics;
};

#endif // LABELATLAS_H
