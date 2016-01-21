#include "mediacontrol.h"
#include <QVBoxLayout>

MediaControl::MediaControl(QWidget *parent) :
    QPushButton(parent),
    icon(new QSvgWidget(this)),
    effect(new QGraphicsColorizeEffect(this))
{
    new QVBoxLayout(this);

    layout()->setMargin(0);
    layout()->addWidget(icon);

    effect->setColor("#b3b3b3");
    icon->setGraphicsEffect(effect);
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

void MediaControl::paintEvent(QPaintEvent *)
{

}

void MediaControl::load(const QString &file)
{
    icon->load(file);
}
