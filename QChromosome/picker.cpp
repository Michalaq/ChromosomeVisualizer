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
        p.setPen("#4d4d4d");
        p.drawRect(0, 0, width() - 1, height() - 1);

        QTextOption opt;
        opt.setAlignment(Qt::AlignVCenter);
        opt.setWrapMode(QTextOption::NoWrap);

        p.drawText(QRect(15, 0, width() - 30, height()), "<< multipe values >>", opt);
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
