#include "slider.h"

Slider::Slider(QWidget *parent) :
    QSlider(parent),
    softMinimum(0),
    softMaximum(0)
{

}

void Slider::setMinimum(int min)
{
    QSlider::setMinimum(min);

    softMinimum = std::max(softMinimum, minimum());
    softMaximum = std::max(softMaximum, minimum());
}

void Slider::setMaximum(int max)
{
    QSlider::setMaximum(max);

    softMinimum = std::min(softMinimum, maximum());
    softMaximum = std::min(softMaximum, maximum());
}

#include <QMouseEvent>
#include <QStyle>

void Slider::mousePressEvent(QMouseEvent *event)
{
    setValue(style()->sliderValueFromPosition(softMinimum, softMaximum, event->pos().x(), width()));
}

void Slider::mouseMoveEvent(QMouseEvent *event)
{
    setValue(style()->sliderValueFromPosition(softMinimum, softMaximum, event->pos().x(), width()));
}

#include <QPainter>

void Slider::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);

    if (minimum() == maximum())
        return;

    QPainter p(this);
    p.setPen(QPen(Qt::white, 2));

    for (int i = softMinimum; i <= softMaximum; i++)
    {
        auto x = style()->sliderPositionFromValue(softMinimum, softMaximum, i, width());
        p.drawLine(QPoint(x,0), QPoint(x,height()));
        p.drawText(QPoint(x,10),QString::number(i));
    }

    if (softMinimum <= value() && value() <= softMaximum)
    {
    auto x = style()->sliderPositionFromValue(softMinimum, softMaximum, value(), width());
    p.fillRect(QRect(x-10,0, 20, height()), Qt::red);
    }
}

void Slider::setSoftMinimum(int min)
{
    softMinimum = std::max(min, minimum());
    update();
}

void Slider::setSoftMaximum(int max)
{
    softMaximum = std::min(max, maximum());
    update();
}
