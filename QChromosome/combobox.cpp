#include "combobox.h"

#include <QStyledItemDelegate>

ComboBox::ComboBox(QWidget *parent) : QComboBox(parent)
{
    setItemDelegate(new QStyledItemDelegate(this));
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
