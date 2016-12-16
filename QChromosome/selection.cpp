#include "selection.h"

Selection::Selection(QWidget *parent) : QWidget(parent), isSelecting(false), isSelectingState(false)
{
    setAttribute(Qt::WA_TransparentForMouseEvents);
}

Selection::~Selection()
{

}

void Selection::setSelectionType(SelectionType t)
{
    st = t;
}

#include <QPaintEvent>
#include <QPainter>

void Selection::paintEvent(QPaintEvent *)
{
    if (isSelecting)
    {
        QPainter p(this);
        p.setPen(Qt::white);

        switch (st)
        {
        case RECTANGULAR_SELECTION:
            p.drawRect(sr);
            break;
        case CUSTOM_SHAPE_SELECTION:
            p.drawPath(path);
            break;
        }
    }
}

#include <QApplication>

void Selection::mousePressEvent(QMouseEvent *event)
{
    QWidget::mousePressEvent(event);

    QApplication::setOverrideCursor(Qt::CrossCursor);

    switch (st)
    {
    case RECTANGULAR_SELECTION:
        tl = br = event->pos();
        sr = QRect(tl, br);
        break;
    case CUSTOM_SHAPE_SELECTION:
        path = QPainterPath();
        path.moveTo(event->pos());
        break;
    }

    isSelecting = true;
    update();

    event->accept();
}

QPoint inbounds(QPoint p, QRect r)
{
    if (p.x() < r.left()) p.setX(r.left());
    if (p.x() > r.right()) p.setX(r.right());

    if (p.y() < r.top()) p.setY(r.top());
    if (p.y() > r.bottom()) p.setY(r.bottom());

    return p;
}

void Selection::mouseMoveEvent(QMouseEvent *event)
{
    QWidget::mouseMoveEvent(event);

    switch (st)
    {
    case RECTANGULAR_SELECTION:
        br = inbounds(event->pos(), rect());
        sr = QRect(tl, br).adjusted(0, 0, -1, -1);
        break;
    case CUSTOM_SHAPE_SELECTION:
        path.lineTo(inbounds(event->pos(), rect()));
        break;
    }

    update();

    event->accept();
}

void Selection::mouseReleaseEvent(QMouseEvent *event)
{
    QWidget::mouseReleaseEvent(event);

    QApplication::restoreOverrideCursor();

    if (st == RECTANGULAR_SELECTION)
    {
        path = QPainterPath();
        path.addRect(sr);
    }

    emit selectionPathChanged(path, event->modifiers());

    isSelecting = false;
    update();

    event->accept();
}

void Selection::enableSelecting(bool b)
{
    isSelectingState = b;
    setAttribute(Qt::WA_TransparentForMouseEvents, !b);
}
