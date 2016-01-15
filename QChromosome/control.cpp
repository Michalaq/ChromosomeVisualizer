#include "control.h"
#include <QVBoxLayout>

Control::Control(QWidget *parent) :
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

Control::~Control()
{

}

void Control::load(const QString &file)
{
    icon->load(file);
}
