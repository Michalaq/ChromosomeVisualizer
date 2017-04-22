#include "combobox.h"

#include <QStyledItemDelegate>

ComboBox::ComboBox(QWidget *parent) : QComboBox(parent)
{
    setItemDelegate(new QStyledItemDelegate(this));
}

void ComboBox::setCurrentIndex(int index, bool spontaneous)
{
    bool b;

    if (!spontaneous)
        b = blockSignals(true);

    QComboBox::setCurrentIndex(index);

    if (!spontaneous)
        blockSignals(b);
}

#include <QStyle>

void ComboBox::focusInEvent(QFocusEvent *event)
{
    QComboBox::focusInEvent(event);

    style()->unpolish(this);
    style()->polish(this);

    update();
}

void ComboBox::focusOutEvent(QFocusEvent *event)
{
    QComboBox::focusOutEvent(event);

    style()->unpolish(this);
    style()->polish(this);

    update();
}
