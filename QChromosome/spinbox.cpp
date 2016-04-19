#include "spinbox.h"

SpinBox::SpinBox(QWidget *parent) : QSpinBox(parent)
{
    cachedMinimumWidth = minimumSizeHint().width();
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

void SpinBox::setMaximum(int max)
{
    setFixedWidth(cachedMinimumWidth + fontMetrics().width(QString::number(max)));
    QSpinBox::setMaximum(max);
}

void SpinBox::setSoftMaximum(int max)
{
    QSpinBox::setMaximum(max);
}
