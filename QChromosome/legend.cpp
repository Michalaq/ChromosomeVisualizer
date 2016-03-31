#include "legend.h"

Legend::Legend(const QString & text, QWidget *parent) : QLabel(text, parent)
{

}

#include <QPainter>

void Legend::paintEvent(QPaintEvent *event)
{
    QLabel::paintEvent(event);

    QPainter painter(this);

    painter.setRenderHint(QPainter::Antialiasing);

    QPen pen(QBrush("#2a7fff"), 2., Qt::SolidLine);
    pen.setCosmetic(true);

    painter.setPen(pen);

    painter.drawEllipse(5, 5, 15, 15);
}
