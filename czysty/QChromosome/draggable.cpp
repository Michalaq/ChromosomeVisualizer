#include "draggable.h"

#include <QGuiApplication>
#include <QScreen>

Draggable::Draggable(QWidget *parent) : QWidget(parent)
{
    updateScreenGeometry(QGuiApplication::primaryScreen()->geometry());

    connect(QGuiApplication::primaryScreen(), SIGNAL(geometryChanged(QRect)), this, SLOT(updateScreenGeometry(QRect)));
}

Draggable::~Draggable()
{

}

#include <QMouseEvent>
#include <QApplication>

void Draggable::mousePressEvent(QMouseEvent *event)
{
    QWidget::mousePressEvent(event);

    initial = event->globalPos();

    QApplication::setOverrideCursor(Qt::BlankCursor);
    QCursor::setPos(center);
}

void Draggable::mouseMoveEvent(QMouseEvent *event)
{
    if (event->globalPos() == center)
        return event->ignore();

    QWidget::mouseMoveEvent(event);

    emit delta(event->globalX() - center.x(), event->globalY() - center.y());

    QCursor::setPos(center);
}

void Draggable::mouseReleaseEvent(QMouseEvent *event)
{
    QWidget::mouseReleaseEvent(event);

    QCursor::setPos(initial);
    QApplication::restoreOverrideCursor();
}

#include <QPainter>
#include <QStyleOption>

void Draggable::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);

    QStyleOption option;
    option.initFrom(this);

    QPainter painter(this);
    style()->drawPrimitive(QStyle::PE_Widget, &option, &painter, this);
}

void Draggable::updateScreenGeometry(QRect geometry)
{
    center = geometry.center();
}
