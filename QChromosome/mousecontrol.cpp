#include "mousecontrol.h"
#include <QGridLayout>

MouseControl::MouseControl(QWidget *parent) :
    Draggable(parent),
    label(new QLabel(this)),
    effect(new QGraphicsColorizeEffect(this))
{
    new QGridLayout(this);

    layout()->setMargin(0);
    layout()->addWidget(label);

    label->setGraphicsEffect(effect);
}

MouseControl::~MouseControl()
{

}

#include <QResizeEvent>

void MouseControl::resizeEvent(QResizeEvent *event)
{
    Draggable::resizeEvent(event);

    label->setPixmap(__icon.pixmap(event->size()));
}

QIcon MouseControl::icon() const
{
    return __icon;
}

void MouseControl::setIcon(const QIcon &icon)
{
    __icon = icon;

    updateGeometry();
}

QColor MouseControl::color() const
{
    return effect->color();
}

void MouseControl::setColor(const QColor &color)
{
    effect->setColor(color);
}
