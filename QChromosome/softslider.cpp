#include "softslider.h"

SoftSlider::SoftSlider(QWidget *parent) :
    QSlider(parent),
    softMinimum(0),
    softMaximum(0)
{

}

SoftSlider::~SoftSlider()
{

}

void SoftSlider::setMinimum(int min)
{
    QSlider::setMinimum(min);

    softMinimum = std::max(softMinimum, minimum());
    softMaximum = std::max(softMaximum, minimum());
}

void SoftSlider::setMaximum(int max)
{
    QSlider::setMaximum(max);

    softMinimum = std::min(softMinimum, maximum());
    softMaximum = std::min(softMaximum, maximum());
}

void SoftSlider::setRange(int min, int max)
{
    QSlider::setRange(min, max);

    setMinimum(minimum());
    setMaximum(maximum());
}

void SoftSlider::setValue(int val, bool spontaneous)
{
    bool b;

    if (!spontaneous)
        b = blockSignals(true);

    QSlider::setValue(val);

    if (!spontaneous)
        blockSignals(b);
}

void SoftSlider::setSoftMinimum(int min)
{
    softMinimum = std::max(min, minimum());
    update();
}

void SoftSlider::setSoftMaximum(int max)
{
    softMaximum = std::min(max, maximum());
    update();
}

#include <QStyle>
#include <QtMath>

int SoftSlider::tickSpan(qreal minsep) const
{
    return tickSpan(minsep, width());
}

int SoftSlider::tickSpan(qreal minsep, int width) const
{
    qreal jmp = minsep * (softMaximum - softMinimum) / width;

    int b = 1;

    while (10 * b <= jmp)
        b *= 10;

    int q = qCeil(jmp / b);

    if (q > 5)
        return 10 * b;

    if (q > 2)
        return 5 * b;

    if (q > 1)
        return 2 * b;

    return b;
}

qreal SoftSlider::tickSpan(qreal min, qreal max, qreal space, qreal minsep)
{
    if (space == 0)
        return qInf();

    qreal jmp = minsep * (max - min) / space;

    qreal b = 1;

    while (b * 10 <= jmp)
        b *= 10;

    if (jmp > 0)
        while (b / 10 >= jmp)
            b /= 10;

    int q = qCeil(jmp / b);

    if (q > 5)
        return b * 10;

    if (q > 2)
        return b * 5;

    if (q > 1)
        return b * 2;

    return b;
}
