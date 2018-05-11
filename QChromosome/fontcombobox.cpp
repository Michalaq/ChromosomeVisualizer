#include "fontcombobox.h"

QFontFamilyStyledDelegate::QFontFamilyStyledDelegate(QObject *parent) : QStyledItemDelegate(parent)
{

}

void QFontFamilyStyledDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyleOptionViewItem opt = option;
    initStyleOption(&opt, index);

    QString text = index.data(Qt::DisplayRole).toString();

    if (QFontDatabase().writingSystems(text).contains(QFontDatabase::Latin))
        opt.font.setFamily(text);

    QStyledItemDelegate::paint(painter, opt, index);
}

FontComboBox::FontComboBox(QWidget *parent) : QFontComboBox(parent), multiple(false)
{
    setItemDelegate(new QFontFamilyStyledDelegate(this));
}

FontComboBox::~FontComboBox()
{

}

void FontComboBox::setMultipleValues()
{
    bool b = blockSignals(true);

    multiple = true;
    QFontComboBox::setCurrentIndex(-1);

    blockSignals(b);
}

void FontComboBox::setCurrentIndex(int index, bool spontaneous)
{
    bool b;

    if (!spontaneous)
        b = blockSignals(true);

    multiple = false;
    QFontComboBox::setCurrentIndex(index);

    if (!spontaneous)
        blockSignals(b);
}

#include <QStyle>

void FontComboBox::focusInEvent(QFocusEvent *event)
{
    QFontComboBox::focusInEvent(event);

    style()->unpolish(this);
    style()->polish(this);

    update();
}

void FontComboBox::focusOutEvent(QFocusEvent *event)
{
    QFontComboBox::focusOutEvent(event);

    style()->unpolish(this);
    style()->polish(this);

    update();
}

#include <QStylePainter>

void FontComboBox::paintEvent(QPaintEvent *)
{
    QStylePainter painter(this);
    painter.setPen(palette().color(QPalette::Text));

    // draw the FontComboBox frame, focusrect and selected etc.
    QStyleOptionComboBox opt;
    initStyleOption(&opt);
    painter.drawComplexControl(QStyle::CC_ComboBox, opt);

    // draw the icon and text
    if (multiple)
        opt.currentText = "<< multiple values >>";

    painter.drawControl(QStyle::CE_ComboBoxLabel, opt);
}
