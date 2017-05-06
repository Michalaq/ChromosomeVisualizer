#include "material.h"
#include "materialrenderer.h"

Material::Material(QWidget *parent) : QWidget(parent), clicked(false)
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
    MaterialRenderer::getInstance()->paint(painter, bounds, this);
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
