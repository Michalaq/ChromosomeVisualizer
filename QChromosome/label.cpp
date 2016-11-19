#include "label.h"

Label::Label(QWidget *parent) : QLabel(parent)
{

}

#include <QPainter>
#include <QFontMetrics>

void Label::paintEvent(QPaintEvent *event)
{
    QLabel::paintEvent(event);

    QPainter p(this);

    auto fm = fontMetrics();

    int offset = fm.width(text());

    int h = height();
    int dx = fm.width(".") + 5;
    int flags = (alignment() & Qt::AlignVertical_Mask) | Qt::AlignRight;

    for (int x = width() - dx; x > offset; x -= dx)
        p.drawText(x, 0, dx, h, flags, ".");
}
