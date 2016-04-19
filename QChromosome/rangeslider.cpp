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
#include <QtMath>

void RangeSlider::mousePressEvent(QMouseEvent *event)
{
    if (maximum() == minimum())
        return event->ignore();

    int pos = event->pos().x();

    if (qAbs(pos - leftHandlePosition) < 10)
    {
        state = LeftHandleMoving;
        return;
    }

    if (qAbs(pos - rightHandlePosition) < 10)
    {
        state = RightHandleMoving;
        return;
    }

    state = IntervalMoving;

    int value = minimum() + qFloor(1. * (maximum() - minimum()) * (event->pos().x() - 20) / (width() - 40) + 0.5);

    relativeLowerBound = lowerBound - value;
    relativeUpperBound = upperBound - value;

    QAbstractSlider::mousePressEvent(event);
}

void RangeSlider::mouseMoveEvent(QMouseEvent *event)
{
    if (maximum() == minimum())
        return event->ignore();

    int value = minimum() + qFloor(1. * (maximum() - minimum()) * (event->pos().x() - 20) / (width() - 40) + 0.5);

    switch (state)
    {
    case LeftHandleMoving:
        setLowerBound(value);
        break;

    case RightHandleMoving:
        setUpperBound(value);
        break;

    case IntervalMoving:
        setBounds(relativeLowerBound + value, relativeUpperBound + value);
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

    if (maximum() == minimum())
        return;

    leftHandlePosition = 20 + (lowerBound - minimum()) * (width() - 40) / (maximum() - minimum()) - 10;
    rightHandlePosition = 20 + (upperBound - minimum()) * (width() - 40) / (maximum() - minimum()) + 10;

    p.fillRect(QRect(leftHandlePosition - 10, -10, rightHandlePosition - leftHandlePosition + 20, 20), "#0044aa");

    p.fillRect(QRect(leftHandlePosition - 5, -5, 10, 10), Qt::white);
    p.fillRect(QRect(rightHandlePosition - 5, -5, 10, 10), Qt::white);

    p.setPen(Qt::white);

    QString leftLabel = QString("%1F").arg(lowerBound);
    QString rightLabel = QString("%1F").arg(upperBound);

    int linewidth = (upperBound - lowerBound) * (width() - 40) / (maximum() - minimum());

    if ((linewidth -= p.fontMetrics().width(leftLabel)) > 0)
        p.drawText(leftHandlePosition + 10, -10, width(), 20, Qt::AlignVCenter | Qt::AlignLeft, leftLabel);

    if ((linewidth -= p.fontMetrics().width(rightLabel)) > 0)
        p.drawText(rightHandlePosition - 10 - width(), -10, width(), 20, Qt::AlignVCenter | Qt::AlignRight, rightLabel);
}

void RangeSlider::setBounds(int min, int max)
{
    if (minimum() <= min && min <= max && max <= maximum())
    {
        if (lowerBound != min)
            emit lowerBoundChanged(lowerBound = min);

        if (upperBound != max)
            emit upperBoundChanged(upperBound = max);

        update();
    }
}

void RangeSlider::setLowerBound(int value)
{
    setBounds(value, upperBound);
}

void RangeSlider::setUpperBound(int value)
{
    setBounds(lowerBound, value);
}

void RangeSlider::setMinimum(int min)
{
    if (upperBound < min)
        setUpperBound(min);

    if (lowerBound < min)
        setLowerBound(min);

    QSlider::setMinimum(min);
}

void RangeSlider::setMaximum(int max)
{
    if (lowerBound > max)
        setLowerBound(max);

    if (upperBound > max)
        setUpperBound(max);

    QSlider::setMaximum(max);
}
