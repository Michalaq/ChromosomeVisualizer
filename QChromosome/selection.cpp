#include "selection.h"

Selection::Selection(QWidget *parent) : QWidget(parent), isSelecting(false), isSelectingState(false)
{

}

Selection::~Selection()
{

}

#include <QEvent>

bool Selection::event(QEvent *event)
{
    if (event->type() == QEvent::MouseButtonPress || event->type() == QEvent::MouseMove || event->type() == QEvent::MouseButtonRelease)
    {
        if (isSelectingState)
        {
            bool b = QWidget::event(event);
            event->accept(); // accept mouse events when selecting and stop propagation
            return b;
        }
        else
        {
            event->ignore(); // ignore mouse events when not selecting and allow propagation
            return false;
        }
    }

    return QWidget::event(event);
}

#include <QPaintEvent>
#include <QPainter>

void Selection::paintEvent(QPaintEvent *)
{
    if (isSelecting)
    {
        QPainter p(this);

        p.setPen(Qt::white);
        p.drawRect(sr.adjusted(0, 0, -1, -1));
    }
}

#include <QApplication>

void Selection::mousePressEvent(QMouseEvent *event)
{
    QWidget::mousePressEvent(event);

    QApplication::setOverrideCursor(Qt::CrossCursor);

    tl = br = event->pos();
    sr = QRect(tl, br).intersected(rect());

    isSelecting = true;
    update();
}

void Selection::mouseMoveEvent(QMouseEvent *event)
{
    QWidget::mouseMoveEvent(event);

    br = event->pos();
    sr = QRect(tl, br).intersected(rect());

    update();
}

void Selection::mouseReleaseEvent(QMouseEvent *event)
{
    QWidget::mouseReleaseEvent(event);

    QApplication::restoreOverrideCursor();

    emit selectionRectChanged(sr, event->modifiers());

    isSelecting = false;
    update();
}

void Selection::enableSelecting(bool b)
{
    isSelectingState = b;
}
