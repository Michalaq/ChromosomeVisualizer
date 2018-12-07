#include "labelatlas.h"

LabelAtlas::LabelAtlas() :
    texture(QOpenGLTexture::Target2D),
    image(1, 1, QImage::Format_RGBA8888),
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

#include <QPainter>

QRect LabelAtlas::addLabel(const Ui::AtomAttributes* ui)
{
    const auto& text = ui->textEdit->toPlainText();

    if (text.isEmpty() || ui->spinBox->value() == 0)
        return QRect();

    auto font = ui->fontComboBox->value();
    font.setPointSize(qAbs(ui->spinBox->value()));

    auto fm = QFontMetrics(font);

    int textWidth = 0;
    int textHeight = 0;

    for (const auto& tmp : text.split('\n'))
    {
        textWidth = qMax(textWidth, fm.width(tmp));
        textHeight += fm.height();
    }

    QRect rect(offset, 0, textWidth += 6, textHeight += 6);
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

    painter.setPen(Qt::NoPen);
    painter.setBrush(ui->comboBox_4->value());

    painter.drawRoundedRect(rect, 3, 3);

    painter.setFont(font);
    painter.setPen(ui->comboBox_6->value());

    static const int flags[] = {Qt::AlignLeft, Qt::AlignCenter, Qt::AlignRight};

    painter.drawText(rect.marginsRemoved({3,3,3,3}), flags[ui->comboBox_3->currentIndex()], text);

    modified = true;

    return rect;
}
