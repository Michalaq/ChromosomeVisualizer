#include "legend.h"

Legend::Legend(const QString & text, const QColor& color, QWidget *parent) :
    QLabel(text, parent),
    _color(color)
{

}

QColor Legend::pen() const
{
    return _color;
}

QColor Legend::brush() const
{
    QColor c(_color);
    c.setAlpha(0x80);
    return c;
}

#include <QPainter>

void Legend::paintEvent(QPaintEvent *event)
{
    QLabel::paintEvent(event);

    QPainter painter(this);

    painter.setRenderHint(QPainter::Antialiasing);

    painter.setPen(QPen(_color, 2));
    painter.setBrush(QBrush(_color));

    painter.drawEllipse(8, 8, 9, 9);
}

#include <QMouseEvent>
#include <QColorDialog>

void Legend::mousePressEvent(QMouseEvent *event)
{
    QLabel::mousePressEvent(event);

    if (event->button() == Qt::LeftButton)
    {
        QColor tmp = QColorDialog::getColor(_color, this);

        if (tmp.isValid())
        {
            _color = tmp;
            update();

            emit changed();
        }
    }
}
