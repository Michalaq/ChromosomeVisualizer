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
    QImage foreground(size(), QImage::Format_ARGB32_Premultiplied);

    QStylePainter stylePainter(&foreground, this);

    QStyleOptionButton option;
    initStyleOption(&option);

    stylePainter.drawPrimitive(QStyle::PE_Widget, option);
    stylePainter.end();

    foreground.setAlphaChannel(icon().pixmap(size(), QIcon::Normal, isChecked() ? QIcon::On : QIcon::Off).toImage().alphaChannel());

    QPainter painter(this);
    painter.drawImage(QRect(QPoint(),iconSize()), foreground);
}
