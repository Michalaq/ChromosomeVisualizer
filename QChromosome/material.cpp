#include "material.h"

Material::Material(QWidget *parent) : QWidget(parent)
{
    setFixedSize(75, 75);
}

QColor Material::getColor() const
{
    return color;
}

void Material::setColor(QColor c)
{
    color = c;
    update();
}

#include <QMimeData>
#include <QDrag>

void Material::mousePressEvent(QMouseEvent *event)
{
    QWidget::mousePressEvent(event);

    QDrag *drag = new QDrag(this);
    drag->setMimeData(new QMimeData);
    drag->exec(Qt::CopyAction | Qt::MoveAction);
}

#include <QPainter>

void Material::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);

    QPainter p(this);
    p.setBrush(color);
    p.drawEllipse(rect());
}
