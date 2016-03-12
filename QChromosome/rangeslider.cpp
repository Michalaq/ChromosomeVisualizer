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
    else
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

    int lowerBoundPosition = QStyle::sliderPositionFromValue(minimum(), maximum(), range.first, width() - 20) + 10;
    int upperBoundPosition = QStyle::sliderPositionFromValue(minimum(), maximum(), range.second, width() - 20) + 10;

    int r;

    if (range.first != range.second)
    {
        p.setBrush(QColor("#0055d4"));

        p.drawRect(lowerBoundPosition, -1, upperBoundPosition - lowerBoundPosition, 2);

        p.setRenderHint(QPainter::Antialiasing);

        r = handle == &range.first ? 9 : 6;
        p.drawEllipse(QPoint(lowerBoundPosition, 0), r, r);

        r = handle == &range.second ? 9 : 6;
        p.drawEllipse(QPoint(upperBoundPosition, 0), r, r);
    }
    else
    {
        p.setPen(QPen(QColor("#666666"), 2));
        p.setBrush(QColor("#1a1a1a"));

        p.setRenderHint(QPainter::Antialiasing);

        r = handle ? 9 : 6;
        p.drawEllipse(QPoint(lowerBoundPosition, 0), r, r);
    }
}
