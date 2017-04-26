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

#include <QDragEnterEvent>

void Material::dragEnterEvent(QDragEnterEvent *event)
{
    event->accept();
}

#include <QPainter>

void Material::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);

    QPainter p(this);
    p.setBrush(color);
    p.drawEllipse(rect());
}
