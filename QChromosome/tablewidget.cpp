#include "tablewidget.h"

TableWidget::TableWidget(QWidget *parent) : QTableWidget(parent)
{

}

void TableWidget::mousePressEvent(QMouseEvent *event)
{
    QTableWidget::mousePressEvent(event);
}

void TableWidget::mouseMoveEvent(QMouseEvent *event)
{
    QTableWidget::mouseMoveEvent(event);
}

void TableWidget::mouseReleaseEvent(QMouseEvent *event)
{
    QTableWidget::mouseReleaseEvent(event);
}

void TableWidget::dragEnterEvent(QDragEnterEvent *event)
{
    QTableWidget::dragEnterEvent(event);
}

void TableWidget::dragMoveEvent(QDragMoveEvent *event)
{
    QTableWidget::dragMoveEvent(event);
}

void TableWidget::dropEvent(QDropEvent *event)
{
    QTableWidget::dropEvent(event);
}

bool TableWidget::event(QEvent *event)
{
    return QTableWidget::event(event);
}
