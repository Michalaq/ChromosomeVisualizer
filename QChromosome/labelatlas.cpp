#include "labelatlas.h"

LabelAtlas::LabelAtlas() :
    fbo(nullptr),
    width(0)
{

}

LabelAtlas::~LabelAtlas()
{
    delete fbo;
}

#include <QString>
void LabelAtlas::initializeGL()
{
    //format.setSamples(16);
    format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);

    fbo = new QOpenGLFramebufferObject({100, 20}, format);

    addLabel("foo");
    addLabel("bar");
}

GLuint LabelAtlas::texture() const
{
    return fbo->texture();
}

#include <QOpenGLPaintDevice>
#include <QPainter>

QRect LabelAtlas::addLabel(const QString &text)
{
    fbo->bind();

    QOpenGLPaintDevice device(fbo->size());

    QPainter painter;
    painter.begin(&device);
    painter.setRenderHint(QPainter::Antialiasing);

    int dx = painter.fontMetrics().width(text) + 10;
    QRect r(width, 0, dx, 20);

    painter.fillRect(r, Qt::green);
    painter.drawText(r, Qt::AlignCenter, text);

    painter.end();

    fbo->release();

    width += dx;
    return r;
}
