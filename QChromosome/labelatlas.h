#ifndef LABELATLAS_H
#define LABELATLAS_H


#include <QOpenGLTexture>
#include <QImage>
#include <QFont>

class LabelAtlas
{
public:
    explicit LabelAtlas();
    ~LabelAtlas();

    void allocate();
    GLuint textureId() const;

    QRect addLabel(const QString& text, const QFont &font);

private:
    QOpenGLTexture texture;
    QImage image;

    int offset;

    bool modified;
};

#endif // LABELATLAS_H
