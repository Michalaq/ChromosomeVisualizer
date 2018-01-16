#include "draggable.h"

Qt::MouseButton Draggable::pressed = Qt::NoButton;

#include <QGuiApplication>
#include <QScreen>

Draggable::Draggable(QWidget *parent) : QPushButton(parent), ignore(false)
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
    event->accept();

    pressed = event->button();

    initial = event->globalPos();

    ignore = true;

    QApplication::setOverrideCursor(Qt::BlankCursor);
    QCursor::setPos(center);
}

void Draggable::mouseMoveEvent(QMouseEvent *event)
{
    event->accept();

    if (ignore)
        ignore = false;
    else
        if (event->globalPos() != center)
        {
            emit delta(event->globalX() - center.x(), event->globalY() - center.y());

            QCursor::setPos(center);
        }
}

void Draggable::mouseReleaseEvent(QMouseEvent *event)
{
    event->accept();

    QCursor::setPos(initial);
    QApplication::restoreOverrideCursor();

    pressed = Qt::NoButton;
}

Qt::MouseButton Draggable::pressedButton()
{
    return pressed;
}

void Draggable::setScreenGeometry(QRect geometry)
{
    center = geometry.center();
}
