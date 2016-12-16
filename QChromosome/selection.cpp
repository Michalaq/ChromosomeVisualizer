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
            p.drawRect(sr.adjusted(0, 0, -1, -1));
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
        sr = QRect(tl, br).intersected(rect());
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

void Selection::mouseMoveEvent(QMouseEvent *event)
{
    QWidget::mouseMoveEvent(event);

    switch (st)
    {
    case RECTANGULAR_SELECTION:
        br = event->pos();
        sr = QRect(tl, br).intersected(rect());
        break;
    case CUSTOM_SHAPE_SELECTION:
        path.lineTo(event->pos());
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
