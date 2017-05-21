#include "material.h"
#include "materialrenderer.h"

Material* Material::dm = nullptr;

Material::Material(QColor c, QColor sc, float se, QWidget *parent) :
    QWidget(parent),
    clicked(false),
    color(c),
    specularColor(sc),
    specularExponent(se)
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
    return specularColor;
}

void Material::setSpecularColor(QColor c)
{
    specularColor = c;
    update();
}

float Material::getSpecularExponent() const
{
    return specularExponent;
}

void Material::setSpecularExponent(qreal e)
{
    specularExponent = e;
    update();
}

Material* Material::getDefault()
{
    return dm ? dm : (dm = new Material);
}

void Material::paint(QPainter *painter, QRect bounds)
{
    painter->save();

    painter->setClipRect(bounds);
    painter->fillRect(bounds, "#999999");

    painter->setRenderHint(QPainter::Antialiasing);

    painter->translate(bounds.center());
    painter->rotate(-45.);

    int size = std::min(bounds.width(), bounds.height());
    qreal step = (qreal)size / 5 / sqrt(2);

    for (int i = -2; i < 3; i++)
        painter->fillRect(QRectF(-size, 2 * i * step, 2 * size, step), "#666666");

    painter->restore();

    size = size * 5 / 6;

    MaterialRenderer::getInstance()->paint(painter, QRect(bounds.x() + (bounds.width() - size) / 2, bounds.y() + (bounds.height() - size) / 2, size, size), this);
}

void Material::assign(const QModelIndex &ix, bool b)
{
    auto it = assignment.find(ix);

    if (b)
    {
        if (it != assignment.end())
            it.value()++;
        else
            assignment.insert(ix, 1);
    }
    else
    {
        if (--it.value() == 0)
            assignment.erase(it);
    }
}

QModelIndexList Material::getAssigned() const
{
    return assignment.keys();
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
