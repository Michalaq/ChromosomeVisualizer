#ifndef LABELATLAS_H
#define LABELATLAS_H


#include <QOpenGLTexture>
#include <QImage>
#include <QFont>

#include "ui_atomattributes.h"

class LabelAtlas
{
public:
    explicit LabelAtlas();
    ~LabelAtlas();

    void allocate();
    GLuint textureId() const;

    QRect addLabel(const Ui::AtomAttributes* ui);

private:
    QOpenGLTexture texture;
    QImage image;

    int offset;

    bool modified;
};

#endif // LABELATLAS_H
