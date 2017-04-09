#include "treeview.h"

TreeView::TreeView(QWidget *parent) : QTreeView(parent)
{

}

#include <QMouseEvent>

void TreeView::mousePressEvent(QMouseEvent *event)
{
    auto index = indexAt(event->pos());

    if (index.column() == 3)
    {
        model()->setData(index, model()->data(index) == "On" ? "Off" : "On", Qt::DisplayRole);
        update();
    }
    else
        QTreeView::mousePressEvent(event);
}
