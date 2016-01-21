#include "mediacontrol.h"

MediaControl::MediaControl(QWidget *parent) :
    QPushButton(parent),
    effect(new QGraphicsColorizeEffect(this))
{
    effect->setColor("#b3b3b3");
    setGraphicsEffect(effect);
}

MediaControl::~MediaControl()
{

}

void MediaControl::enterEvent(QEvent *event)
{
    QPushButton::enterEvent(event);

    effect->setColor("#ffffff");
}

void MediaControl::leaveEvent(QEvent *event)
{
    QPushButton::leaveEvent(event);

    if (!isChecked())
        effect->setColor("#b3b3b3");
}

#include <QPainter>

void MediaControl::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    icon().paint(&painter, rect());
}
