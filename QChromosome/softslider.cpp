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
