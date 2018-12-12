#ifndef LABELATLAS_H
#define LABELATLAS_H


#include <QOpenGLTexture>
#include <QImage>

#include "ui_atomattributes.h"

class LabelAtlas : public QOpenGLTexture
{
public:
    explicit LabelAtlas();
    ~LabelAtlas();

    void allocate();

    QRect addLabel(const Ui::AtomAttributes* ui);

private:
    QImage image;

    int offset;

    bool modified;
};

#endif // LABELATLAS_H
