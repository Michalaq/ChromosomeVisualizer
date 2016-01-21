#include "mousecontrol.h"
#include <QVBoxLayout>

MouseControl::MouseControl(QWidget *parent) :
    Draggable(parent),
    icon(new QSvgWidget(this)),
    effect(new QGraphicsColorizeEffect(this))
{
    new QVBoxLayout(this);

    layout()->setMargin(0);
    layout()->addWidget(icon);

    icon->setGraphicsEffect(effect);
}

MouseControl::~MouseControl()
{

}

QColor MouseControl::getColor() const
{
    return effect->color();
}

void MouseControl::setColor(const QColor &c)
{
    effect->setColor(c);
}

void MouseControl::setIcon(const QString &file)
{
    icon->load(file);
}
