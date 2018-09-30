#include "slider.h"

Slider::Slider(QWidget *parent) : QSlider(parent)
{

}

#include <QMouseEvent>
#include <QStyle>

void Slider::mousePressEvent(QMouseEvent* ev)
{
    ev->accept();

    switch (orientation())
    {
    case Qt::Horizontal:
        setValue(style()->sliderValueFromPosition(minimum(), maximum(), ev->pos().x(), width()));
        break;
    case Qt::Vertical:
        setValue(style()->sliderValueFromPosition(minimum(), maximum(), ev->pos().y(), height()));
        break;
    }
}

void Slider::mouseMoveEvent(QMouseEvent* ev)
{
    mousePressEvent(ev);
}
