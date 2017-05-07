#include "material.h"
#include "materialrenderer.h"

Material::Material(QWidget *parent) : QWidget(parent), clicked(false)
{
    setFixedSize(45, 45);
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

QColor Material::getSpecularColor() const
{
    return Qt::white;
}

void Material::setSpecularColor(QColor c)
{
    ;
}

float Material::getSpecularExponent() const
{
    return 10.;
}

void Material::setSpecularExponent(qreal e)
{

}

void Material::paint(QPainter *painter, QRect bounds)
{
    /*painter->save();

    painter->setClipRect(bounds);
    painter->fillRect(bounds, "#999999");

    painter->setRenderHint(QPainter::Antialiasing);

    painter->translate(bounds.center());
    painter->rotate(-45.);

    int size = std::min(bounds.width(), bounds.height());
    qreal step = (qreal)size / 5 / sqrt(2);

    for (int i = -2; i < 3; i++)
        painter->fillRect(QRectF(-size, 2 * i * step, 2 * size, step), "#666666");

    painter->restore();*/

    int size = std::min(bounds.width(), bounds.height()) * 5 / 6;

    MaterialRenderer::getInstance()->paint(painter, QRect(bounds.x() + (bounds.width() - size) / 2, bounds.y() + (bounds.height() - size) / 2, size, size), this);
}

void Material::mousePressEvent(QMouseEvent *event)
{
    clicked = true;

    QWidget::mousePressEvent(event);
}

#include <QMimeData>
#include <QDrag>

void Material::mouseMoveEvent(QMouseEvent *event)
{
    if (clicked)
    {
        clicked = false;

        QDrag *drag = new QDrag(this);
        drag->setMimeData(new QMimeData);
        drag->exec(Qt::CopyAction | Qt::MoveAction);
    }
    else
        QWidget::mouseMoveEvent(event);
}

#include <QPainter>

void Material::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);

    QPainter p(this);
    paint(&p, rect());
}
