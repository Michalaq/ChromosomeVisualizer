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
    return effect->color();
}

void MouseControl::setColor(const QColor &color)
{
    effect->setColor(color);
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
