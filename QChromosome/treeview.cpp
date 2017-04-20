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
    scene->setTreeView(this);
}

Visibility TreeView::getVisibility(const QList<unsigned int> &indexes) const
{
    auto indices = static_cast<TreeModel*>(model())->getIndices();

    auto ans = getVisibility(indices[indexes.first()]);

    for (unsigned int i : indexes)
        if (getVisibility(indices[i]) != ans)
            return Default;

    return ans;
}

void TreeView::setVisibility(const QList<unsigned int> &indexes, Visibility v)
{
    auto indices = static_cast<TreeModel*>(model())->getIndices();

    for (unsigned int i : indexes)
        setVisibility(indices[i], v);

    update();
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

Visibility TreeView::getVisibility(const QModelIndex &root) const
{
    auto i = vie.find(root);
    return i != vie.end() ? *i ? On : Off : Default;
}

void TreeView::setVisibility(const QModelIndex &root, Visibility v)
{
    model()->setData(root.sibling(root.row(), 3), v, Qt::DisplayRole);

    vie.remove(root);

    QList<unsigned int> id;
    dumpModel(root, id);

    auto root_ = root;

    if (v != Default)
        vie[root_] = (v == On);
    else
    {
        while (!vie.contains(root_))
            root_ = root_.parent();
    }

    scene->customSelection(id).setVisible_(vie[root_]);
}

#include <QMouseEvent>

void TreeView::mousePressEvent(QMouseEvent *event)
{
    auto index = indexAt(event->pos());

    if (index.column() == 3)
    {
        state = (model()->data(index).toInt() + 1) % 3;
        setVisibility(index.sibling(index.row(), 0), Visibility(state));
        if (selectionModel()->isSelected(index))
            emit vieChanged();
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
            if (selectionModel()->isSelected(index))
                emit vieChanged();
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
