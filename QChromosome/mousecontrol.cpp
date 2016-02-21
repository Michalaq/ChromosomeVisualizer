#include "mousecontrol.h"

MouseControl::MouseControl(QWidget *parent) :
    Draggable(parent),
    effect(new QGraphicsColorizeEffect(this))
{
    setGraphicsEffect(effect);
}

MouseControl::~MouseControl()
{

}

QIcon MouseControl::icon() const
{
    return _icon;
}

void MouseControl::setIcon(const QIcon &icon)
{
    _icon = icon;
}

QColor MouseControl::color() const
{
    return effect->color();
}

void MouseControl::setColor(const QColor &color)
{
    effect->setColor(color);
}

#include <QPainter>

void MouseControl::paintEvent(QPaintEvent *event)
{
    Draggable::paintEvent(event);

    QPainter painter(this);

    _icon.paint(&painter, rect());
}
