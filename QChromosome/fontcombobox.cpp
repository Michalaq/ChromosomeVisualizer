#include "fontcombobox.h".h"

FontComboBox::FontComboBox(QWidget *parent) : ComboBox(parent)
{

}

FontComboBox::~FontComboBox()
{

}

const QFont& FontComboBox::value() const
{
    return font;
}

void FontComboBox::setValue(const QFont& f, bool spontaneous)
{
    font = f;

    if (spontaneous)
        emit valueChanged(font);

    update();
}

#include <QStylePainter>

void FontComboBox::paintEvent(QPaintEvent *)
{
    QStylePainter painter(this);
    painter.setPen(palette().color(QPalette::Text));

    // draw the combobox frame, focusrect and selected etc.
    QStyleOptionComboBox opt;
    initStyleOption(&opt);
    opt.currentText = font.family();
    painter.drawComplexControl(QStyle::CC_ComboBox, opt);

    // draw the icon and text
    painter.drawControl(QStyle::CE_ComboBoxLabel, opt);
}

#include <QFontDialog>

void FontComboBox::mousePressEvent(QMouseEvent *event)
{
    QWidget::mousePressEvent(event);

    bool ok;
    QFont f = QFontDialog::getFont(&ok, font);

    if (ok && f != font)
        setValue(f);
}
