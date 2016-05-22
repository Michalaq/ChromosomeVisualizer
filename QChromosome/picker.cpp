#include "picker.h"

Picker::Picker(QWidget *parent) : QWidget(parent)
{

}

void Picker::setValue(const QVariant& c)
{
    color = c;
    update();
}

#include <QPainter>

void Picker::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);

    QPainter p(this);

    if (color.isValid())
        p.fillRect(rect(), color.value<QColor>());
    else
    {
        p.setPen(Qt::lightGray);
        p.drawText(rect(), "<< multipe values >>", QTextOption(Qt::AlignVCenter));
    }
}

#include <QColorDialog>

void Picker::mousePressEvent(QMouseEvent *event)
{
    QWidget::mousePressEvent(event);

    QColor c = QColorDialog::getColor(color.value<QColor>());

    if (c.isValid() && c != color)
    {
        emit valueChanged(c);

        color = c;
        update();
    }
}
