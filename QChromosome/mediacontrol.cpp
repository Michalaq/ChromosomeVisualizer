#include "mediacontrol.h"

MediaControl::MediaControl(QWidget *parent) :
    QPushButton(parent),
    effect(new QGraphicsColorizeEffect(this))
{
    effect->setColor(Qt::transparent);
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

#include <QPainter>

void MediaControl::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    icon().paint(&painter, rect(), Qt::AlignCenter, QIcon::Normal, isChecked() ? QIcon::On : QIcon::Off);
}
