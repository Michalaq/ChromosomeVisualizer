#include "material.h"
#include <QFile>

Material::Material(QWidget *parent) : QWidget(parent), clicked(false)
{
    setFixedSize(75, 75);

    QFile file(":/sphere");
    file.open(QFile::ReadOnly | QFile::Text);

    svg = file.readAll();

    file.close();

    specular_color_index = svg.indexOf("#ffffff");
    color_index = svg.indexOf("#000000");
}

QColor Material::getColor() const
{
    return color;
}

void Material::setColor(QColor c)
{
    color = c;
    svg.replace(color_index, 7, color.name().toUtf8());
    update();
}

#include <QSvgRenderer>

void Material::paint(QPainter *painter, QRect bounds)
{
    QSvgRenderer(svg).render(painter, bounds);
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
