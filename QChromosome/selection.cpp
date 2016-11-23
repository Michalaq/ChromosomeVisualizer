#include "selection.h"

Selection::Selection(QWidget *parent) : QWidget(parent), isSelecting(false)
{
    enableSelecting(false);
}

Selection::~Selection()
{

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
    setAttribute(Qt::WA_NoMousePropagation, b);
    setAttribute(Qt::WA_TransparentForMouseEvents, !b);
}
