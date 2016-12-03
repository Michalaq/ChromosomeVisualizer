#include "axis.h"

Axis::Axis(QWidget *parent) :
    QWidget(parent),
    textVisible(true),
    scale(1),
    position(BottomLeft)
{
    setAttribute(Qt::WA_TransparentForMouseEvents);
}

#include <QPainter>

void Axis::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);

    if (position == Off)
        return;

    struct
    {
        QVector3D vector;
        Qt::GlobalColor color;
        QChar label;
    }
    axis[] =
    {
        {modelView.mapVector({1, 0, 0}), Qt::red,   'X'},
        {modelView.mapVector({0, 1, 0}), Qt::green, 'Y'},
        {modelView.mapVector({0, 0, 1}), Qt::blue,  'Z'}
    };

    if (axis[0].vector.z() > axis[1].vector.z()) std::swap(axis[0], axis[1]);
    if (axis[1].vector.z() > axis[2].vector.z()) std::swap(axis[1], axis[2]);
    if (axis[0].vector.z() > axis[1].vector.z()) std::swap(axis[0], axis[1]);

    QRectF r(0, 0, 20, 20);

    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    p.translate(position == TopLeft || position == BottomLeft ? 50 * scale : width() - 50 * scale,
                position == TopLeft || position == TopRight ? 50 * scale : height() - 50 * scale);
    p.scale(scale, scale);

    for (auto a : axis)
    {
        p.setPen(QPen(a.color, 2, Qt::SolidLine, Qt::RoundCap));
        p.drawLine({0,0}, QPointF(a.vector.x(), -a.vector.y()) * 30);

        if (textVisible)
        {
            r.moveCenter(QPointF(a.vector.x(), -a.vector.y()) * 40);
            p.drawText(r, Qt::AlignCenter, a.label);
        }
    }
}

void Axis::setModelView(const QMatrix4x4 &mat)
{
    modelView = mat;
    update();
}

void Axis::setTextVisible(bool czy)
{
    textVisible = czy;
    update();
}

void Axis::setScale(double s)
{
    scale = s;
    update();
}

void Axis::setPosition(int p)
{
    position = p;
    update();
}
