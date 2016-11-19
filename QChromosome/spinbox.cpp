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

#include <QMouseEvent>
#include <QStyleOptionSpinBox>

void SpinBox::mousePressEvent(QMouseEvent *event)
{
    QSpinBox::mousePressEvent(event);

    QStyleOptionSpinBox opt;
    initStyleOption(&opt);

    if (style()->subControlRect(QStyle::CC_SpinBox, &opt, QStyle::SC_SpinBoxUp).contains(event->pos())
            || style()->subControlRect(QStyle::CC_SpinBox, &opt, QStyle::SC_SpinBoxDown).contains(event->pos()))
        emit editingFinished();
}
