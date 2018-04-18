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

void LabelAtlas::initializeGL()
{
    initializeOpenGLFunctions();

    format.setSamples(16);
    format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);

    fbo = new QOpenGLFramebufferObject({1000, 20}, format);
}

void LabelAtlas::paintGL()
{
    glBindTexture(GL_TEXTURE_2D, fbo->texture());
}

#include <QOpenGLPaintDevice>
#include <QPainter>

QRect LabelAtlas::addLabel(const QString &text)
{
    fbo->bind();

    QOpenGLPaintDevice device(fbo->size());
    QPainter painter(&device);

    int dx = painter.fontMetrics().width(text) + 10;
    QRect r(width, 0, dx, 20);

    painter.fillRect(r, Qt::green);
    painter.drawText(r, Qt::AlignCenter, text);

    fbo->release();

    width += dx;
    return r;
}
