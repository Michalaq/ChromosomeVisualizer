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

void PickWidget::pick(QPersistentModelIndex object)
{
    if (object.isValid())
    {
        obj = object;
        update();
    }
}

void PickWidget::mousePressEvent(QMouseEvent *event)
{
    LineEdit::mousePressEvent(event);

    sm.map(this);
}
