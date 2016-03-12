#include "rangeslider.h"

RangeSlider::RangeSlider(QWidget *parent) :
    QSlider(parent),
    lowerBound(0),
    upperBound(0),
    activeHandle(NoHandle)
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

    if (2 * value < lowerBound + upperBound)
    {
        activeHandle = LowerBoundHandle;
        setLowerBound(value);
    }
    else
    {
        activeHandle = UpperBoundHandle;
        setUpperBound(value);
    }
}

void RangeSlider::mouseMoveEvent(QMouseEvent *event)
{
    QAbstractSlider::mouseMoveEvent(event);

    if (!activeHandle)
        return event->ignore();

    int value = QStyle::sliderValueFromPosition(minimum(), maximum(), event->pos().x() - 10, width() - 20);

    if (activeHandle == LowerBoundHandle)
        setLowerBound(value);

    if (activeHandle == UpperBoundHandle)
        setUpperBound(value);
}

void RangeSlider::mouseReleaseEvent(QMouseEvent *event)
{
    QAbstractSlider::mouseReleaseEvent(event);

    activeHandle = NoHandle;
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

    int lowerBoundPosition = QStyle::sliderPositionFromValue(minimum(), maximum(), lowerBound, width() - 20) + 10;
    int upperBoundPosition = QStyle::sliderPositionFromValue(minimum(), maximum(), upperBound, width() - 20) + 10;

    int r;

    if (lowerBound != upperBound)
    {
        p.setBrush(QColor("#0055d4"));

        p.drawRect(lowerBoundPosition, -1, upperBoundPosition - lowerBoundPosition, 2);

        p.setRenderHint(QPainter::Antialiasing);

        r = activeHandle == LowerBoundHandle ? 9 : 6;
        p.drawEllipse(QPoint(lowerBoundPosition, 0), r, r);

        r = activeHandle == UpperBoundHandle ? 9 : 6;
        p.drawEllipse(QPoint(upperBoundPosition, 0), r, r);
    }
    else
    {
        p.setPen(QPen(QColor("#666666"), 2));
        p.setBrush(QColor("#1a1a1a"));

        p.setRenderHint(QPainter::Antialiasing);

        r = activeHandle ? 9 : 6;
        p.drawEllipse(QPoint(lowerBoundPosition, 0), r, r);
    }
}

void RangeSlider::setLowerBound(int value)
{
    if (minimum() <= value && value <= upperBound)
    {
        lowerBound = value;
        update();
    }
}

void RangeSlider::setUpperBound(int value)
{
    if (lowerBound <= value && value <= maximum())
    {
        upperBound = value;
        update();
    }
}
