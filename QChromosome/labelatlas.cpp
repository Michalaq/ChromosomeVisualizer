#include "labelatlas.h"

LabelAtlas::LabelAtlas() :
    fbo(nullptr),
    pos(0),
    size(1),
    font(":/fonts/Roboto-Regular"),
    fmetrics(font)
{
    //format.setSamples(16);
    format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
}

LabelAtlas::~LabelAtlas()
{
    delete fbo;
}

void LabelAtlas::initializeGL()
{
    fbo = new QOpenGLFramebufferObject({1, 20}, format);
}

GLuint LabelAtlas::texture() const
{
    return fbo->texture();
}

#include <QOpenGLPaintDevice>
#include <QPainter>

QRect LabelAtlas::addLabel(const QString &text)
{
    if (text.isEmpty())
        return QRect();

    int width = fmetrics.width(text) + 10;
    QRect rect(pos, 0, width, 20);

    while (pos + width > size)
        size *= 2;

    if (fbo->width() < size)
    {
        auto tmp = new QOpenGLFramebufferObject({size, 20}, format);
        QOpenGLFramebufferObject::blitFramebuffer(tmp, {0, 0, pos, 20}, fbo, {0, 0, pos, 20});

        delete fbo;
        fbo = tmp;
    }

    fbo->bind();

    QOpenGLPaintDevice device(fbo->size());

    QPainter painter;

    painter.begin(&device);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.setFont(font);

    painter.fillRect(rect, Qt::green);
    painter.drawText(rect, Qt::AlignCenter, text);

    painter.end();

    fbo->release();

    pos += width;
    return rect;
}
