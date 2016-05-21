#include "picker.h"

Picker::Picker(QWidget *parent) : QWidget(parent)
{

}

#include <QPainter>

void Picker::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);

    QPainter p(this);
    p.fillRect(rect(), color);
}

#include <QColorDialog>

void Picker::mousePressEvent(QMouseEvent *event)
{
    QWidget::mousePressEvent(event);

    QColor c = QColorDialog::getColor(color);

    if (c.isValid())
    {
        color = c;
        update();
    }
}
