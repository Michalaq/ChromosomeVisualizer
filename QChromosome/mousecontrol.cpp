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

    label->setPixmap(icon().pixmap(size()));
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
    return __color;
}

void MouseControl::setColor(const QColor &color)
{
    effect->setColor(__color = color);
}

QColor MouseControl::pressed() const
{
    return __pressed;
}

void MouseControl::setPressed(const QColor &color)
{
    __pressed = color;
}

void MouseControl::mousePressEvent(QMouseEvent *event)
{
    Draggable::mousePressEvent(event);

    effect->setColor(pressed());
}

void MouseControl::mouseReleaseEvent(QMouseEvent *event)
{
    Draggable::mouseReleaseEvent(event);

    effect->setColor(color());
}

#include <QPainter>
#include <QStyleOption>

void MouseControl::paintEvent(QPaintEvent *event)
{
    Draggable::paintEvent(event);

    QStyleOption option;
    option.initFrom(this);

    QPainter painter(this);

    style()->drawPrimitive(QStyle::PE_Widget, &option, &painter, this);
}
