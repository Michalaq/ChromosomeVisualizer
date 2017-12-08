#include "pickwidget.h"

QSignalMapper PickWidget::sm;

PickWidget::PickWidget(QWidget *parent) : LineEdit(parent)
{
    sm.setMapping(this, this);
}

const QSignalMapper& PickWidget::getSignalMapper()
{
    return sm;
}

void PickWidget::pick(QModelIndex object)
{
    obj = object;
}

void PickWidget::mousePressEvent(QMouseEvent *event)
{
    LineEdit::mousePressEvent(event);

    sm.map(this);
}
