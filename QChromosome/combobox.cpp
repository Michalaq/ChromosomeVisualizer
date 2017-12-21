#include "combobox.h"

#include <QStyledItemDelegate>

ComboBox::ComboBox(QWidget *parent) : QComboBox(parent), multiple(false)
{
    setItemDelegate(new QStyledItemDelegate(this));

    connect(this, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [this] {
        multiple = false;
    });
}

ComboBox::~ComboBox()
{

}

void ComboBox::setCurrentIndex(int index, bool spontaneous)
{
    bool b;

    if (!spontaneous)
        b = blockSignals(true);

    if (index > std::numeric_limits<int>::lowest())
        QComboBox::setCurrentIndex(index);
    else
    {
        QComboBox::setCurrentIndex(-1);
        multiple = true;
        update();
    }

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
    if (multiple)
        opt.currentText = "<< multiple values >>";

    painter.drawControl(QStyle::CE_ComboBoxLabel, opt);
}
