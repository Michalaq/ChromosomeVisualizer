#include "rangeslider.h"

RangeSlider::RangeSlider(QWidget *parent) :
    QSlider(parent),
    lowerBound(0),
    upperBound(0),
    leftHandlePosition(10),
    rightHandlePosition(30),
    state(Normal)
{

}

QSize RangeSlider::minimumSizeHint() const
{
    return QSize(40, 20);
}

int RangeSlider::getLowerBound() const
{
    return lowerBound;
}

int RangeSlider::getUpperBound() const
{
    return upperBound;
}

#include <QMouseEvent>

void RangeSlider::mousePressEvent(QMouseEvent *event)
{
    if (maximum() == minimum())
        return event->ignore();

    initialPosition = event->pos().x();

    if (qAbs(initialPosition - leftHandlePosition) < 10)
    {
        state = LeftHandleMoving;
        return;
    }

    if (qAbs(initialPosition - rightHandlePosition) < 10)
    {
        state = RightHandleMoving;
        return;
    }

    state = IntervalMoving;

    QAbstractSlider::mousePressEvent(event);
}

void RangeSlider::mouseMoveEvent(QMouseEvent *event)
{
    if (maximum() == minimum())
        return event->ignore();

    int position = event->pos().x();

    switch (state)
    {
    case LeftHandleMoving:
        setLowerBound(1. * (maximum() - minimum()) * (position - 10) / (width() - 40) + 0.5);
        break;

    case RightHandleMoving:
        setUpperBound(1. * (maximum() - minimum()) * (position - 30) / (width() - 40) + 0.5);
        break;

    case IntervalMoving:
        int value = 1. * (position - initialPosition) * (maximum() - minimum()) / (width() - 40) + (position > initialPosition ? 0.5 : -0.5);

        if (lowerBound + value >= minimum() && upperBound + value <= maximum())
        {
            lowerBound += value;
            upperBound += value;
            update();

            emit lowerBoundChanged(lowerBound);
            emit upperBoundChanged(upperBound);
        }

        initialPosition += value * (width() - 40) / (maximum() - minimum());

        break;
    }

    QAbstractSlider::mouseMoveEvent(event);
}

void RangeSlider::mouseReleaseEvent(QMouseEvent *event)
{
    state = Normal;
    update();

    QAbstractSlider::mouseReleaseEvent(event);
}

#include <QPainter>

void RangeSlider::paintEvent(QPaintEvent *event)
{
    QAbstractSlider::paintEvent(event);

    QPainter p(this);
    p.translate(0, height() / 2);

    p.fillRect(QRect(0, -10, width(), 20), "#262626");

    bool null = maximum() == minimum();

    leftHandlePosition = null ? 10 : lowerBound * (width() - 40) / (maximum() - minimum()) + 10;
    rightHandlePosition = null ? 30 : upperBound * (width() - 40) / (maximum() - minimum()) + 30;

    p.fillRect(QRect(leftHandlePosition - 10, -10, rightHandlePosition - leftHandlePosition + 20, 20), "#0044aa");

    p.fillRect(QRect(leftHandlePosition - 5, -5, 10, 10), Qt::white);
    p.fillRect(QRect(rightHandlePosition - 5, -5, 10, 10), Qt::white);

    if (null) return;

    p.setPen(Qt::white);

    QString leftLabel = QString("%1F").arg(lowerBound);
    QString rightLabel = QString("%1F").arg(upperBound);

    int linewidth = (upperBound - lowerBound) * (width() - 40) / (maximum() - minimum());

    if ((linewidth -= p.fontMetrics().width(leftLabel)) > 0)
        p.drawText(leftHandlePosition + 10, -10, width(), 20, Qt::AlignVCenter | Qt::AlignLeft, leftLabel);

    if ((linewidth -= p.fontMetrics().width(rightLabel)) > 0)
        p.drawText(rightHandlePosition - 10 - width(), -10, width(), 20, Qt::AlignVCenter | Qt::AlignRight, rightLabel);
}

void RangeSlider::setLowerBound(int value)
{
    if (minimum() <= value && value <= upperBound)
    {
        lowerBound = value;
        update();

        emit lowerBoundChanged(value);
    }
}

void RangeSlider::setUpperBound(int value)
{
    if (lowerBound <= value && value <= maximum())
    {
        upperBound = value;
        update();

        emit upperBoundChanged(value);
    }
}

void RangeSlider::setMinimum(int min)
{
    QSlider::setMinimum(min);
}

void RangeSlider::setMaximum(int max)
{
    QSlider::setMaximum(max);
}
