#include "picker.h"

Picker::Picker(QWidget *parent) : QWidget(parent)
{

}

void Picker::setInitialColor(const QColor &c)
{
    color = c;
    update();
}

#include <QPainter>

void Picker::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);

    QPainter(this).fillRect(rect(), color);
}

#include <QColorDialog>

void Picker::mousePressEvent(QMouseEvent *event)
{
    QWidget::mousePressEvent(event);

    QColor c = QColorDialog::getColor(color);

    if (c.isValid() && c != color)
    {
        emit valueChanged(c);

        color = c;
        update();
    }
}
