#include "spinbox.h"

SpinBox::SpinBox(QWidget *parent) : QSpinBox(parent)
{

}

#include <QStyle>

void SpinBox::focusInEvent(QFocusEvent *event)
{
    QSpinBox::focusInEvent(event);

    style()->unpolish(this);
    style()->polish(this);

    update();
}

void SpinBox::focusOutEvent(QFocusEvent *event)
{
    QSpinBox::focusOutEvent(event);

    style()->unpolish(this);
    style()->polish(this);

    update();
}
