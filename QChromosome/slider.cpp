#include "slider.h"

Slider::Slider(QWidget *parent) : SoftSlider(parent)
{

}

QSize Slider::minimumSizeHint() const
{
    return QSize(20, 30);
}

#include <QMouseEvent>
#include <QStyle>

void Slider::mousePressEvent(QMouseEvent *event)
{
    setValue(style()->sliderValueFromPosition(softMinimum, softMaximum, event->pos().x() - 10, width() - 20));
}

void Slider::mouseMoveEvent(QMouseEvent *event)
{
    setValue(style()->sliderValueFromPosition(softMinimum, softMaximum, event->pos().x() - 10, width() - 20));
}

#include <QPainter>
#include <QtMath>

void Slider::paintEvent(QPaintEvent *event)
{
    QAbstractSlider::paintEvent(event);

    QPainter p(this);
    p.translate(0, height() / 2);

    p.fillRect(QRect(0, -12, width(), 24), "#262626");

    if (softMinimum == softMaximum)
        return;

    p.setPen("#4d4d4d");

    p.drawLine(QPoint(0, 0), QPoint(width(), 0));

    p.setPen(Qt::white);

    int gap = tickSpan(p.fontMetrics().width(QString::number(softMaximum)) + 20);

    for (int i = (gap - (softMinimum % gap)) % gap; i <= softMaximum - softMinimum; i += gap)
    {
        auto tick = style()->sliderPositionFromValue(softMinimum, softMaximum, softMinimum + i, width() - 20) + 10;

        p.drawLine(QPoint(tick, 0), QPoint(tick, 12));
        p.drawText(QRect(tick, -12, 0, 12), Qt::AlignCenter | Qt::TextDontClip, QString::number(softMinimum + i));
    }

    int sgap = gap >= 10 ? gap / 10 : 1;

    for (int i = (sgap - (softMinimum % sgap)) % sgap; i <= softMaximum - softMinimum; i += sgap)
    {
        if (((softMinimum + i) % gap) == 0)
            continue;

        auto tick = style()->sliderPositionFromValue(softMinimum, softMaximum, softMinimum + i, width() - 20) + 10;

        p.drawLine(QPoint(tick, 6), QPoint(tick, 12));
    }

    if (softMinimum <= value() && value() <= softMaximum)
    {
        auto tick = style()->sliderPositionFromValue(softMinimum, softMaximum, value(), width() - 20) + 10;

        p.setPen("#0072bd");

        p.fillRect(QRect(tick, -12, p.fontMetrics().width(QString::number(value())) + 10, 12), "#262626");
        p.drawText(QRect(tick + 10, -12, 0, 12), Qt::AlignVCenter | Qt::TextDontClip, QString::number(value()));

        p.setPen(Qt::NoPen);
        p.setBrush(QColor("#0072bd"));
        p.setRenderHint(QPainter::Antialiasing);

        p.translate(tick, (1. - qSqrt(2.)) * 5);
        p.drawEllipse(QPoint(), 10, 10);
        p.rotate(45);
        p.drawRect(0, 0, 10, 10);
    }
}
