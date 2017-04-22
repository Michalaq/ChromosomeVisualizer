#include "rangeslider.h"

RangeSlider::RangeSlider(QWidget *parent) :
    QSlider(parent),
    lowerBound(0),
    upperBound(0),
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
#include <QStyle>

void RangeSlider::mousePressEvent(QMouseEvent *event)
{
    if (maximum() == minimum())
        return event->ignore();

    int value = style()->sliderValueFromPosition(minimum(), maximum(), event->pos().x() - 20, width() - 40);

    if (value <= lowerBound)
    {
        state = LeftHandleMoving;
        return setLowerBound(value);
    }

    if (value >= upperBound)
    {
        state = RightHandleMoving;
        return setUpperBound(value);
    }

    state = IntervalMoving;

    relativeLowerBound = lowerBound - value;
    relativeUpperBound = upperBound - value;

    QAbstractSlider::mousePressEvent(event);
}

void RangeSlider::mouseMoveEvent(QMouseEvent *event)
{
    if (maximum() == minimum())
        return event->ignore();

    int value = style()->sliderValueFromPosition(minimum(), maximum(), event->pos().x() - 20, width() - 40);

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

    int leftHandle = 20 + style()->sliderPositionFromValue(minimum(), maximum(), lowerBound, width() - 40) - 10;
    int rightHandle = 20 + style()->sliderPositionFromValue(minimum(), maximum(), upperBound, width() - 40) + 10;

    p.fillRect(QRect(leftHandle - 10, -10, rightHandle - leftHandle + 20, 20), "#0044aa");

    p.fillRect(QRect(leftHandle - 5, -5, 10, 10), Qt::white);
    p.fillRect(QRect(rightHandle - 5, -5, 10, 10), Qt::white);

    p.setPen(Qt::white);

    QString leftLabel = QString("%1F").arg(lowerBound);
    QString rightLabel = QString("%1F").arg(upperBound);

    int linewidth = style()->sliderPositionFromValue(0, maximum() - minimum(), upperBound - lowerBound, width() - 40);

    if ((linewidth -= p.fontMetrics().width(leftLabel)) > 0)
        p.drawText(leftHandle + 10, -10, width(), 20, Qt::AlignVCenter | Qt::AlignLeft, leftLabel);

    if ((linewidth -= p.fontMetrics().width(rightLabel)) > 0)
        p.drawText(rightHandle - 10 - width(), -10, width(), 20, Qt::AlignVCenter | Qt::AlignRight, rightLabel);
}

void RangeSlider::setBounds(int min, int max, bool spontaneous)
{
    if (minimum() <= min && min <= max && max <= maximum())
    {
        if (lowerBound != min)
        {
            lowerBound = min;

            if (spontaneous)
                emit lowerBoundChanged(lowerBound);
        }

        if (upperBound != max)
        {
            upperBound = max;

            if (spontaneous)
                emit upperBoundChanged(upperBound);
        }

        update();
    }
}

void RangeSlider::setLowerBound(int value, bool spontaneous)
{
    setBounds(value, upperBound, spontaneous);
}

void RangeSlider::setUpperBound(int value, bool spontaneous)
{
    setBounds(lowerBound, value, spontaneous);
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
