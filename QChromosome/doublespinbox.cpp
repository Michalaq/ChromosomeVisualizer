#include "doublespinbox.h"

DoubleSpinBox::DoubleSpinBox(QWidget *parent) : QDoubleSpinBox(parent)
{

}

#include <QStyle>

void DoubleSpinBox::focusInEvent(QFocusEvent *event)
{
    QDoubleSpinBox::focusInEvent(event);

    style()->unpolish(this);
    style()->polish(this);

    update();
}

void DoubleSpinBox::focusOutEvent(QFocusEvent *event)
{
    QDoubleSpinBox::focusOutEvent(event);

    style()->unpolish(this);
    style()->polish(this);

    update();
}
