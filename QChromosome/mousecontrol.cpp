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

    effect->setColor("#cccccc");
    icon->setGraphicsEffect(effect);
}

MouseControl::~MouseControl()
{

}

void MouseControl::load(const QString &file)
{
    icon->load(file);
}
