#include "draggable.h"

Qt::MouseButton Draggable::pressed = Qt::NoButton;

#include <QGuiApplication>
#include <QScreen>

Draggable::Draggable(QWidget *parent) : QWidget(parent)
{
    setScreenGeometry(QGuiApplication::primaryScreen()->geometry());

    connect(QGuiApplication::primaryScreen(), SIGNAL(geometryChanged(QRect)), this, SLOT(setScreenGeometry(QRect)));
}

Draggable::~Draggable()
{

}

#include <QMouseEvent>
#include <QApplication>

void Draggable::mousePressEvent(QMouseEvent *event)
{
    QWidget::mousePressEvent(event);

    state |= QStyle::State_Sunken;
    update();

    pressed = event->button();

    initial = event->globalPos();

    QApplication::setOverrideCursor(Qt::BlankCursor);
    QCursor::setPos(center);
}

void Draggable::mouseMoveEvent(QMouseEvent *event)
{
    QWidget::mouseMoveEvent(event);

    if (event->globalPos() != center)
    {
        emit delta(event->globalX() - center.x(), event->globalY() - center.y());

        QCursor::setPos(center);
    }
}

void Draggable::mouseReleaseEvent(QMouseEvent *event)
{
    QWidget::mouseReleaseEvent(event);

    QCursor::setPos(initial);
    QApplication::restoreOverrideCursor();

    pressed = Qt::NoButton;

    state &= ~QStyle::State_Sunken;
}

#include <QPainter>
#include <QStyleOption>

void Draggable::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);

    QStyleOption option;
    option.initFrom(this);
    option.state |= state;

    QPainter painter(this);
    style()->drawPrimitive(QStyle::PE_Widget, &option, &painter, this);
}

Qt::MouseButton Draggable::pressedButton()
{
    return pressed;
}

void Draggable::setScreenGeometry(QRect geometry)
{
    center = geometry.center();
}
