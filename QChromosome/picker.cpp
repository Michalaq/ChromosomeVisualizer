#include "picker.h">

Picker::Picker(QWidget *parent) : ComboBox(parent)
{
    addItem(QIcon(), "<< multiple values >>");
}

void Picker::setValue(const QVariant& c)
{
    color = c;

    setItemText(0, color.isValid() ? color.value<QColor>().name() : "<< multiple values >>");

    update();
}

#include <QStylePainter>

void Picker::paintEvent(QPaintEvent *)
{
    QStylePainter painter(this);
    painter.setPen(palette().color(QPalette::Text));

    // draw the combobox frame, focusrect and selected etc.
    QStyleOptionComboBox opt;
    initStyleOption(&opt);
    painter.drawComplexControl(QStyle::CC_ComboBox, opt);

    // draw color indicator
    if (color.isValid())
    {
        painter.translate(22, 0);
        QPainter(this).fillRect(5, 5, 22, 22, color.value<QColor>());
    }

    // draw the icon and text
    painter.drawControl(QStyle::CE_ComboBoxLabel, opt);
}

#include <QColorDialog>

void Picker::mousePressEvent(QMouseEvent *event)
{
    QWidget::mousePressEvent(event);

    QColor c = QColorDialog::getColor(color.value<QColor>());

    if (c.isValid() && c != color)
    {
        setValue(c);
        emit valueChanged(c);
    }
}
