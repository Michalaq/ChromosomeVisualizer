#include "doublespinbox.h"

DoubleSpinBox::DoubleSpinBox(QWidget *parent) : QDoubleSpinBox(parent)
{
    setKeyboardTracking(false);
}

void DoubleSpinBox::setValue(double val, bool spontaneous)
{
    bool b;

    if (!spontaneous)
        b = blockSignals(true);

    if (qIsNaN(val))
    {
        setSpecialValueText("<< multiple values >>");
        QDoubleSpinBox::setValue(minimum());
    }
    else
    {
        setSpecialValueText("");
        QDoubleSpinBox::setValue(val);
    }

    if (!spontaneous)
        blockSignals(b);
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
