#include "rangeslider.h"

RangeSlider::RangeSlider(QWidget *parent) :
    QSlider(parent),
    handle(nullptr)
{

}

QSize RangeSlider::minimumSizeHint() const
{
    return QSize(20, 20);
}

#include <QMouseEvent>
#include <QStyle>

void RangeSlider::mousePressEvent(QMouseEvent *event)
{
    QAbstractSlider::mousePressEvent(event);

    if (maximum() == minimum() || (event->buttons() ^ event->button()))
        return event->ignore();

    int value = QStyle::sliderValueFromPosition(minimum(), maximum(), event->pos().x() - 10, width() - 20);

    handle = 2 * value < range.first + range.second ? &range.first : &range.second;

    *handle = value;

    update();
}

void RangeSlider::mouseMoveEvent(QMouseEvent *event)
{
    QAbstractSlider::mouseMoveEvent(event);

    int value = QStyle::sliderValueFromPosition(minimum(), maximum(), event->pos().x() - 10, width() - 20);

    std::swap(*handle, value);

    if (range.first > range.second)
        std::swap(*handle, value);

    update();
}

void RangeSlider::mouseReleaseEvent(QMouseEvent *event)
{
    QAbstractSlider::mouseReleaseEvent(event);

    handle = nullptr;

    update();
}

#include <QPainter>

void RangeSlider::paintEvent(QPaintEvent *event)
{
    QAbstractSlider::paintEvent(event);

    QPainter p(this);
    p.translate(0, height() / 2);

    p.setPen(Qt::NoPen);
    p.setBrush(QColor("#666666"));

    p.drawRect(10, -1, width() - 20, 2);

    p.setBrush(QColor("#0055d4"));

    p.drawRect(QStyle::sliderPositionFromValue(minimum(), maximum(), range.first, width() - 20) + 10, -1, QStyle::sliderPositionFromValue(minimum(), maximum(), range.second - range.first, width() - 20), 2);

    p.setRenderHint(QPainter::Antialiasing);

    if (range.first==range.second)
    {
        p.setPen(QPen(QColor("#666666"), 2));
        p.setBrush(QColor("#1a1a1a"));
    }

    p.drawEllipse(QPoint(QStyle::sliderPositionFromValue(minimum(), maximum(), range.first, width() - 20) + 10, 0), handle==&range.first||(range.first==range.second&&handle) ? 9 : 6,  handle==&range.first||(range.first==range.second&&handle) ? 9 : 6);

    if (range.first!=range.second)
    p.drawEllipse(QPoint(QStyle::sliderPositionFromValue(minimum(), maximum(), range.second, width() - 20) + 10, 0), handle==&range.second ? 9 : 6,  handle==&range.second ? 9 : 6);

    /*if (handle){
        p.setRenderHint(QPainter::Antialiasing, false);
        p.translate(10+*handle*(width()-20)/(maximum()-minimum()),0);



        p.setPen(QColor("#767676"));
        p.setBrush(QColor("#2b2b2b"));
        p.drawRect(QRect(-30, -30, 60, 20));


   p.setRenderHint(QPainter::Antialiasing);
    p.setPen(Qt::white);
    p.drawText(QRect(-20, -20-10, 40, 20), Qt::AlignCenter, QString::number(*handle));


    }*/
    //p.drawText(QRect(value.second*width()/(maximum()-minimum())-20, -16-10, 40, 20), Qt::AlignCenter, QString::number(value.second));
    //p.drawText(QPoint(value.first*width()/(maximum()-minimum())+12, -5), QString::number(value.first));
}
