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

Visibility TreeView::getVisibility(const QList<unsigned int> &indexes, VisibilityMode m) const
{
    auto indices = static_cast<TreeModel*>(model())->getIndices();

    auto ans = getVisibility(indices[indexes.first()], m);

    for (unsigned int i : indexes)
        if (getVisibility(indices[i], m) != ans)
            return Default;

    return ans;
}

void TreeView::setVisibility(const QList<unsigned int> &indexes, Visibility v, VisibilityMode m)
{
    auto indices = static_cast<TreeModel*>(model())->getIndices();

    for (unsigned int i : indexes)
        setVisibility(indices[i], v, m);

    update();
}

void TreeView::dumpModel(const QModelIndex& root, QList<unsigned int>& id, VisibilityMode m)
{
    bool ok;
    unsigned int i = root.sibling(root.row(), 2).data().toUInt(&ok);

    if (ok)
        id.append(i);

    for (int r = 0; r < model()->rowCount(root); r++)
    {
        auto c = root.child(r, 0);

        if (getVisibility(c, m) == Default)
            dumpModel(c, id, m);
    }
}

Visibility TreeView::getVisibility(const QModelIndex &root, VisibilityMode m) const
{
    return root.isValid() ? Visibility(model()->data(root.sibling(root.row(), m)).toInt()) : On;
}

void TreeView::setVisibility(const QModelIndex &root, Visibility v, VisibilityMode m)
{
    model()->setData(root.sibling(root.row(), m), v, Qt::DisplayRole);

    QList<unsigned int> id;
    dumpModel(root.sibling(root.row(), 0), id, m);

    auto root_ = root;

    while (getVisibility(root_, m) == Default)
        root_ = root_.parent();

    if (m == Editor)
        scene->customSelection(id).setVisible_(getVisibility(root_, m) == On);
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
            vm = event->pos().y() < visualRect(index).center().y() + 3 ? Editor : Renderer;
            cv = Visibility((getVisibility(index, vm) + 1) % 3);

            setVisibility(index, cv, vm);

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
            setVisibility(index, cv, vm);

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
