#include "treeview.h"

TreeView::TreeView(QWidget *parent) :
    QTreeView(parent),
    state(-1)
{

}

TreeView::~TreeView()
{

}

#include "treemodel.h"

void TreeView::setSelection(const QList<unsigned int> &indexes)
{
    auto t = static_cast<TreeModel*>(model());
    auto indices = t->getIndices();

    QItemSelection m;

    for (unsigned int i : indexes)
        m.select(indices[i], indices[i]);

    bool b = blockSignals(true);
    selectionModel()->select(m, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
    blockSignals(b);
}

#include <QMouseEvent>

void TreeView::mousePressEvent(QMouseEvent *event)
{
    auto index = indexAt(event->pos());

    if (index.column() == 3)
    {
        state = (model()->data(index).toInt() + 1) % 3;
        model()->setData(index, state, Qt::DisplayRole);
        update();
    }
    else
        QTreeView::mousePressEvent(event);
}

void TreeView::mouseMoveEvent(QMouseEvent *event)
{
    if (state != -1)
    {
        setCursor(Qt::DragCopyCursor);

        auto index = indexAt(event->pos());

        if (index.column() == 3)
        {
            model()->setData(index, state, Qt::DisplayRole);
            update();
        }
    }
    else
        QTreeView::mouseMoveEvent(event);
}

void TreeView::mouseReleaseEvent(QMouseEvent *event)
{
    QTreeView::mouseReleaseEvent(event);

    if (state != -1)
    {
        unsetCursor();
        state = -1;
    }
}
