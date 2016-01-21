#include "mousecontrol.h"
#include <QVBoxLayout>

MouseControl::MouseControl(QWidget *parent) :
    Draggable(parent),
    svg(new QSvgWidget(this)),
    effect(new QGraphicsColorizeEffect(this))
{
    new QVBoxLayout(this);

    layout()->setMargin(0);
    layout()->addWidget(svg);

    svg->setGraphicsEffect(effect);
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

QString MouseControl::icon() const
{
    return "";
}

void MouseControl::setIcon(const QString &icon)
{
    svg->load(icon);
}
