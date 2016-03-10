#include "rangeslider.h"

RangeSlider::RangeSlider(QWidget *parent) : QSlider(parent)
{

}

#include <QPainter>

void RangeSlider::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.translate(0, 0.5 * height());

    p.setPen(Qt::NoPen);
    p.setBrush(QColor("#666666"));

    p.drawRect(0, -1, width(), 2);

    p.setBrush(QColor("#0055d4"));

    p.drawRect(100, -1, 400, 2);

    p.setRenderHint(QPainter::Antialiasing);

    p.drawEllipse(QPoint(100, 0), 6, 6);
    p.drawEllipse(QPoint(500, 0), 6, 6);
}
