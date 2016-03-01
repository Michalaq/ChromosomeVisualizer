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

QColor MouseControl::color() const
{
    return effect->color();
}

void MouseControl::setColor(const QColor &color)
{
    effect->setColor(color);
}
