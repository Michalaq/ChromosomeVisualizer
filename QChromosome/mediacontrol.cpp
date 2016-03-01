#include "mediacontrol.h"

MediaControl::MediaControl(QWidget *parent) :
    QPushButton(parent),
    effect(new QGraphicsColorizeEffect(this))
{
    setGraphicsEffect(effect);
}

MediaControl::~MediaControl()
{

}

QColor MediaControl::color() const
{
    return effect->color();
}

void MediaControl::setColor(const QColor &color)
{
    effect->setColor(color);
}
