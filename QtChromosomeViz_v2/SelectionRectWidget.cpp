#include "SelectionRectWidget.hpp"

SelectionRectWidget::SelectionRectWidget(QWidget * parent)
    : QWidget(parent)
{
    setAttribute(Qt::WA_TransparentForMouseEvents);
}

void SelectionRectWidget::setRectangle(const QRect & r)
{
    QRegion region;
    region += rect_;
    region += r;
    update(r);

    rect_ = r;
}

void SelectionRectWidget::paintEvent(QPaintEvent * event)
{
    QBrush brush(QColor(255, 255, 255, 160));
    QPainter painter(this);

    painter.fillRect(rect_, brush);
    painter.setPen(Qt::white);
    painter.setBrush(Qt::transparent);
    painter.drawRect(rect_);
}
