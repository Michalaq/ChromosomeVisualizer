#include "picker.h">

Picker::Picker(QWidget *parent) : ComboBox(parent)
{
    addItem(QIcon(), "<< multiple values >>");
}

QColor Picker::value() const
{
    return color;
}

void Picker::setValue(const QColor& c, bool spontaneous)
{
    color = c;
    setItemText(0, color.isValid() ? color.name() : "<< multiple values >>");

    if (spontaneous)
        emit valueChanged(color);

    update();
}

void Picker::showAlphaChannel(bool c)
{
    alpha = c;
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
        // draw alpha chessboard
        for (int i = 0; i < 4; i++)
            for (int j = 0; j < 4; j++)
                painter.fillRect(4 + 6 * i, 4 + 6 * j, 6, 6, (i + j) % 2 ? "#666666" : "#999999");

        painter.translate(28, 0);
        QPainter(this).fillRect(4, 4, 24, 24, color);
    }

    // draw the icon and text
    painter.drawControl(QStyle::CE_ComboBoxLabel, opt);
}

#include <QColorDialog>

void Picker::mousePressEvent(QMouseEvent *event)
{
    QWidget::mousePressEvent(event);

    QColor c = QColorDialog::getColor(color, Q_NULLPTR, QString(), alpha ? QColorDialog::ShowAlphaChannel : QColorDialog::ColorDialogOptions());

    if (c.isValid() && c != color)
        setValue(c);
}
