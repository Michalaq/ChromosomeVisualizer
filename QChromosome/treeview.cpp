#include "treeview.h"

TreeView::TreeView(QWidget *parent) :
    QTreeView(parent),
    state(-1)
{
    vie[QModelIndex()] = true;
}

TreeView::~TreeView()
{

}

void TreeView::setSelection(const QList<unsigned int> &indexes)
{
    auto indices = static_cast<TreeModel*>(model())->getIndices();

    QItemSelection m;

    for (unsigned int i : indexes)
        m.select(indices[i], indices[i]);

    bool b = blockSignals(true);
    selectionModel()->select(m, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
    blockSignals(b);
}

void TreeView::setScene(VizWidget *s)
{
    scene = s;
}

void TreeView::setVisibility(const QList<unsigned int> &indexes, Visibility v)
{
    auto indices = static_cast<TreeModel*>(model())->getIndices();

    for (unsigned int i : indexes)
        setVisibility(indices[i], v);
}

void TreeView::dumpModel(const QModelIndex& root, QList<unsigned int>& id)
{
    bool ok;
    unsigned int i = root.sibling(root.row(), 2).data().toUInt(&ok);

    if (ok)
        id.append(i);

    for (int r = 0; r < model()->rowCount(root); r++)
    {
        auto c = root.child(r, 0);

        if (!vie.contains(c))
            dumpModel(c, id);
    }
}

void TreeView::setVisibility(QModelIndex root, Visibility v)
{
    model()->setData(root.sibling(root.row(), 3), v, Qt::DisplayRole);

    vie.remove(root);

    QList<unsigned int> id;
    dumpModel(root, id);

    if (v != Default)
        vie[root] = (v == On);
    else
    {
        while (!vie.contains(root))
            root = root.parent();
    }

    scene->customSelection(id).setVisible(vie[root]);
}

#include <QMouseEvent>

void TreeView::mousePressEvent(QMouseEvent *event)
{
    auto index = indexAt(event->pos());

    if (index.column() == 3)
    {
        state = (model()->data(index).toInt() + 1) % 3;
        setVisibility(index.sibling(index.row(), 0), Visibility(state));
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
            setVisibility(index.sibling(index.row(), 0), Visibility(state));
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
