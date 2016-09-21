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

#include <QMouseEvent>
#include <QStyleOptionSpinBox>

void DoubleSpinBox::mousePressEvent(QMouseEvent *event)
{
    QDoubleSpinBox::mousePressEvent(event);

    QStyleOptionSpinBox opt;
    initStyleOption(&opt);

    if (style()->subControlRect(QStyle::CC_SpinBox, &opt, QStyle::SC_SpinBoxUp).contains(event->pos())
            || style()->subControlRect(QStyle::CC_SpinBox, &opt, QStyle::SC_SpinBoxDown).contains(event->pos()))
        emit editingFinished();
}
