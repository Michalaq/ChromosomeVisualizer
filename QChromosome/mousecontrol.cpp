#include "mousecontrol.h"

MouseControl::MouseControl(QWidget *parent) : Draggable(parent)
{

}

MouseControl::~MouseControl()
{

}

#include <QStylePainter>
#include <QStyleOptionButton>

void MouseControl::paintEvent(QPaintEvent *event)
{
    QImage foreground(iconSize(), QImage::Format_ARGB32_Premultiplied);

    QStylePainter stylePainter(&foreground, this);

    QStyleOptionButton option;
    initStyleOption(&option);

    stylePainter.drawPrimitive(QStyle::PE_Widget, option);
    stylePainter.end();

    foreground.setAlphaChannel(icon().pixmap(iconSize(), QIcon::Normal, isChecked() ? QIcon::On : QIcon::Off).toImage().alphaChannel());

    QRect foregroundRect = foreground.rect();
    foregroundRect.moveCenter(rect().center());

    QPainter painter(this);
    painter.drawImage(foregroundRect, foreground);
}
