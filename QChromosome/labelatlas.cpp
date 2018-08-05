#include "labelatlas.h"

LabelAtlas::LabelAtlas() :
    texture(QOpenGLTexture::Target2D),
    image(1, 1, QImage::Format_ARGB32),
    offset(0),
    modified(true)
{

}

LabelAtlas::~LabelAtlas()
{

}

void LabelAtlas::allocate()
{
    if (modified)
    {
        texture.destroy();
        texture.setData(image, QOpenGLTexture::DontGenerateMipMaps);

        modified = false;
    }
}

GLuint LabelAtlas::textureId() const
{
    return texture.textureId();
}

QSize LabelAtlas::size() const
{
    return image.size();
}

#include <QPainter>

QRect LabelAtlas::addLabel(const QString &text, const QFont& font)
{
    if (text.isEmpty())
        return QRect();

    auto fm = QFontMetrics(font);

    int textWidth = fm.width(text);
    int textHeight = fm.height();

    QRect rect(offset, 0, textWidth, textHeight);
    offset += textWidth;

    int width = image.width();

    while (offset > width)
        width *= 2;

    QSize size = image.size().expandedTo(QSize(width, textHeight));

    if (image.size() != size)
        image = image.copy(QRect(QPoint(), size));

    QPainter painter(&image);

    painter.translate(0, textHeight);
    painter.scale(1, -1);

    painter.setFont(font);
    painter.setPen(Qt::green);

    painter.drawText(rect, Qt::AlignCenter, text);

    modified = true;

    return rect;
}
