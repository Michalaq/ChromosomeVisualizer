#include "SelectionRectWidget.hpp"

SelectionRectWidget::SelectionRectWidget(QWidget * parent)
    : QWidget(parent)
{
    setAttribute(Qt::WA_TransparentForMouseEvents);
}

void SelectionRectWidget::setRectangle(const QRectF & r)
{
    rect_ = r;
    update();
}

void SelectionRectWidget::paintEvent(QPaintEvent * event)
{
    QBrush brush(Qt::gray, Qt::Dense4Pattern);
    QPainter painter(this);

    painter.fillRect(rect_, brush);
    painter.setPen(Qt::white);
    painter.setBrush(Qt::transparent);
    painter.drawRect(rect_);
}
