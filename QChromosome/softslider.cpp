#include "softslider.h"

SoftSlider::SoftSlider(QWidget *parent) :
    QSlider(parent),
    softMinimum(0),
    softMaximum(0)
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

int SoftSlider::tickSpan(qreal sep) const
{
    qreal jmp = sep * (softMaximum - softMinimum) / width();

    int b = 1;

    while (10 * b <= jmp)
        b *= 10;

    int q = qCeil(jmp / b);

    if (q > 5)
        return 10 * b;
    else
    {
        if (q > 2)
            return 5 * b;
        else
        {
            if (q > 1)
                return 2 * b;
            else
                return 1 * b;
        }
    }
}
