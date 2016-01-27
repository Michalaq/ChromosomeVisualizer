#include "mediacontrol.h"

MediaControl::MediaControl(QWidget *parent) :
    QPushButton(parent),
    __color(Qt::transparent),
    __hover(Qt::transparent),
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
    return __color;
}

void MediaControl::setColor(const QColor &c)
{
    effect->setColor(__color = c);
}

QColor MediaControl::hover() const
{
    return __hover;
}

void MediaControl::setHover(const QColor &c)
{
    __hover = c;
}

void MediaControl::click()
{
    QPushButton::click();

    effect->setColor(color());
}

void MediaControl::enterEvent(QEvent *event)
{
    QPushButton::enterEvent(event);

    effect->setColor(hover());
}

void MediaControl::leaveEvent(QEvent *event)
{
    QPushButton::leaveEvent(event);

    if (!isChecked())
        effect->setColor(color());
}

#include <QPainter>

void MediaControl::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    icon().paint(&painter, rect(), Qt::AlignCenter, QIcon::Normal, isChecked() ? QIcon::On : QIcon::Off);
}
