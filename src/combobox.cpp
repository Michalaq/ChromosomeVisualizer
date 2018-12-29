#include "combobox.h"

#include <QStyledItemDelegate>

ComboBox::ComboBox(QWidget *parent) : QComboBox(parent)
{
    setItemDelegate(new QStyledItemDelegate(this));
}

ComboBox::~ComboBox()
{

}

void ComboBox::setMultipleValues()
{
    setCurrentIndex(-1, false);
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

#include <QStylePainter>

void ComboBox::paintEvent(QPaintEvent *)
{
    QStylePainter painter(this);
    painter.setPen(palette().color(QPalette::Text));

    // draw the combobox frame, focusrect and selected etc.
    QStyleOptionComboBox opt;
    initStyleOption(&opt);
    painter.drawComplexControl(QStyle::CC_ComboBox, opt);

    // draw the icon and text
    if (currentIndex() == -1)
        opt.currentText = "<< multiple values >>";

    painter.drawControl(QStyle::CE_ComboBoxLabel, opt);
}
