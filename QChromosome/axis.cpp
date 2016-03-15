#include "axis.h"

Axis::Axis(QWidget *parent) : QWidget(parent)
{
    setAttribute(Qt::WA_TransparentForMouseEvents);
}

#include <QPainter>

void Axis::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);

    struct
    {
        QVector3D vector;
        Qt::GlobalColor color;
        QChar label;
    }
    axis[] =
    {
        {modelView.mapVector({1,0,0}), Qt::red, 'X'},
        {modelView.mapVector({0,1,0}), Qt::green, 'Y'},
        {modelView.mapVector({0,0,1}), Qt::blue, 'Z'}
    };

    if (axis[0].vector.z() < axis[1].vector.z()) std::swap(axis[0], axis[1]);
    if (axis[1].vector.z() < axis[2].vector.z()) std::swap(axis[1], axis[2]);
    if (axis[0].vector.z() < axis[1].vector.z()) std::swap(axis[0], axis[1]);

    QRectF r(0, 0, 20, 20);

    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    p.translate(50, height() - 50);

    for (auto a : axis)
    {
        p.setPen(QPen(a.color, 2));
        p.drawLine({0,0}, a.vector.toPointF() * 30);

        r.moveCenter(a.vector.toPointF() * 40);
        p.drawText(r, Qt::AlignCenter, a.label);
    }
}

void Axis::setModelView(const QMatrix4x4 &mat)
{
    modelView = mat;
    update();
}
