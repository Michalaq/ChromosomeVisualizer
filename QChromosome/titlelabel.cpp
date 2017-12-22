#include "titlelabel.h"

TitleLabel::TitleLabel(QWidget *parent) : QLabel(parent)
{

}

void TitleLabel::setTitle(const QString &t)
{
    title = t;
    update();
}

void TitleLabel::setElements(const QString &e)
{
    elements = e;
    update();
}

#include <QPainter>

void TitleLabel::paintEvent(QPaintEvent *)
{
    QPainter(this).drawText(0, 0, width(), height(), alignment(), title + "[" + fontMetrics().elidedText(elements, Qt::ElideRight, width() - fontMetrics().width(title + "[]")) + "]");
}
