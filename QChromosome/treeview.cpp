#include "treeview.h"

TreeView::TreeView(QWidget *parent) :
    QTreeView(parent),
    state(NoState)
{
    setMouseTracking(true);
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

        if (getVisibility(c) == Default)
            dumpModel(c, id);
    }
}

Visibility TreeView::getVisibility(const QModelIndex &root) const
{
    return root.isValid() ? Visibility(model()->data(root.sibling(root.row(), 3)).toInt()) : On;
}

void TreeView::setVisibility(const QModelIndex &root, Visibility v)
{
    model()->setData(root.sibling(root.row(), 3), v, Qt::DisplayRole);

    QList<unsigned int> id;
    dumpModel(root.sibling(root.row(), 0), id);

    auto root_ = root;

    while (getVisibility(root_) == Default)
        root_ = root_.parent();

    scene->customSelection(id).setVisible_(getVisibility(root_) == On);
}

#include <QMouseEvent>

void TreeView::mousePressEvent(QMouseEvent *event)
{
    if (qAbs(event->pos().x() - columnWidth(0)) < 4)
    {
        state = ResizeSection;
        cp = event->pos().x();
        cw = columnWidth(0);
    }
    else
    {
        auto index = indexAt(event->pos());

        if (index.column() == 3)
        {
            state = ChangeVisibility;
            cv = Visibility((model()->data(index).toInt() + 1) % 3);

            setVisibility(index, cv);

            if (selectionModel()->isSelected(index))
                emit vieChanged();

            update();
        }
        else
            QTreeView::mousePressEvent(event);
    }
}

void TreeView::mouseMoveEvent(QMouseEvent *event)
{
    switch (state)
    {
    case NoState:
        if (qAbs(event->pos().x() - columnWidth(0)) < 4)
            setCursor(Qt::SplitHCursor);
        else
            if (testAttribute(Qt::WA_SetCursor))
                unsetCursor();

        QTreeView::mouseMoveEvent(event);
        break;

    case ResizeSection:
        setColumnWidth(0, qMax(cw + event->pos().x() - cp, header()->minimumSectionSize()));
        break;

    case ChangeVisibility:
        if (!testAttribute(Qt::WA_SetCursor))
            setCursor(Qt::DragCopyCursor);

        auto index = indexAt(event->pos());

        if (index.column() == 3)
        {
            setVisibility(index, cv);

            if (selectionModel()->isSelected(index))
                emit vieChanged();

            update();
        }
    }
}

void TreeView::mouseReleaseEvent(QMouseEvent *event)
{
    switch (state)
    {
    case NoState:
        QTreeView::mouseReleaseEvent(event);
        break;

    case ResizeSection:
        state = NoState;
        unsetCursor();
        break;

    case ChangeVisibility:
        state = NoState;
        unsetCursor();
        break;
    }
}
