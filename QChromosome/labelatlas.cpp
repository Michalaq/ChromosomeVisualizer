#include "labelatlas.h"

const int height = 32;

LabelAtlas::LabelAtlas() :
    fbo(nullptr),
    pos(0),
    width(1),
    font(":/fonts/Roboto-Regular")
{
    font.setPixelSize(height);

    //format.setSamples(16);
    format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
}

LabelAtlas::~LabelAtlas()
{
    delete fbo;
}

void LabelAtlas::initializeGL()
{
    fbo = new QOpenGLFramebufferObject({1, height}, format);
}

GLuint LabelAtlas::texture() const
{
    return fbo->texture();
}

QRect LabelAtlas::size() const
{
    return QRect(0, 0, width, height);
}

#include <QOpenGLPaintDevice>
#include <QPainter>

QRect LabelAtlas::addLabel(const QString &text)
{
    if (text.isEmpty())
        return QRect();

    int width_ = QFontMetrics(font).width(text);
    QRect rect(pos, 0, width_, height);

    while (pos + width_ > width)
        width *= 2;

    if (fbo->width() < width)
    {
        auto tmp = new QOpenGLFramebufferObject({width, height}, format);

        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        QOpenGLFramebufferObject::blitFramebuffer(tmp, {0, 0, pos, height}, fbo, {0, 0, pos, height});

        delete fbo;
        fbo = tmp;
    }

    fbo->bind();

    QOpenGLPaintDevice device(fbo->size());

    QPainter painter;

    painter.begin(&device);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.setFont(font);
    painter.setPen(Qt::green);

    painter.fillRect(rect, Qt::transparent);
    painter.drawText(rect, Qt::AlignCenter, text);

    painter.end();

    fbo->release();

    pos += width_;
    return rect;
}
