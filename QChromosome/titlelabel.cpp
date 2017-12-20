#include "titlelabel.h"

TitleLabel::TitleLabel(QWidget *parent) : QLabel(parent)
{

}

void TitleLabel::setContents(const QString &t, const QString &l)
{
    title = t;
    list = l;
    update();
}

#include <QPainter>

void TitleLabel::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    QPainter(this).drawText(0, 0, width(), height(), alignment(), title + "[" + fontMetrics().elidedText(list, Qt::ElideRight, width() - fontMetrics().width(title + "[]")) + "]");
}
