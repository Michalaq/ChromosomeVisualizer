#include "treeview.h"

TreeView::TreeView(QWidget *parent) : QTreeView(parent)
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

    selectionModel()->select(m, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);

    update();
}

#include <QMouseEvent>

void TreeView::mousePressEvent(QMouseEvent *event)
{
    auto index = indexAt(event->pos());

    if (index.column() == 3)
    {
        state = model()->data(index) == "On" ? "Off" : "On";
        model()->setData(index, state, Qt::DisplayRole);
        update();
    }
    else
        QTreeView::mousePressEvent(event);
}

void TreeView::mouseMoveEvent(QMouseEvent *event)
{
    if (state.isValid())
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

    if (state.isValid())
    {
        unsetCursor();
        state.clear();
    }
}
