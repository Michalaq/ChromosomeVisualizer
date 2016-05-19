#include "blind.h"

Blind::Blind(QWidget *parent) :
    QWidget(parent),
    aspectRatio(1)
{
    setAttribute(Qt::WA_TransparentForMouseEvents);
}

#include <QPainter>
#include <QtMath>

void Blind::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);

    int w = std::min(width(), int(qreal(height()) * aspectRatio + 0.5));
    int h = std::min(height(), int(qreal(width()) / aspectRatio + 0.5));

    QRect view(0, 0, w, h);
    view.moveCenter(rect().center());

    QPainter p(this);
    p.setOpacity(0.75);

    p.fillRect(QRect(rect().topLeft(), view.bottomLeft() - QPoint(1, 0)), "#1a1a1a");
    p.fillRect(QRect(rect().topLeft(), view.topRight() - QPoint(0, 1)), "#1a1a1a");

    p.fillRect(QRect(view.topRight() + QPoint(1, 0), rect().bottomRight()), "#1a1a1a");
    p.fillRect(QRect(view.bottomLeft() + QPoint(0, 1), rect().bottomRight()), "#1a1a1a");
}

void Blind::setAspectRatio(qreal ar)
{
    aspectRatio = ar;
    update();
}
