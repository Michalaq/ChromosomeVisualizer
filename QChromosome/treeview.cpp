#include "treeview.h"
#include "../QtChromosomeViz_v2/VizWidget.hpp"

TreeView::TreeView(QWidget *parent) :
    QTreeView(parent),
    state(NoState)
{
    setHeader(hv = new HeaderView(header()->orientation(), this));
    hv->setStretchLastSection(true);
    setMouseTracking(true);
    setAcceptDrops(true);
}

TreeView::~TreeView()
{

}

HeaderView::HeaderView(Qt::Orientation orientation, QWidget *parent) : QHeaderView(orientation, parent)
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

QVariant TreeView::getName(const QList<unsigned int> &indexes) const
{
    auto indices = static_cast<TreeModel*>(model())->getIndices();

    auto ans = model()->data(indices[indexes.first()]).toString();

    for (unsigned int i : indexes)
        if (model()->data(indices[i]).toString() != ans)
            return QVariant();

    return ans;
}

void TreeView::setName(const QList<unsigned int> &indexes, const QString &name)
{
    auto indices = static_cast<TreeModel*>(model())->getIndices();

    for (unsigned int i : indexes)
        model()->setData(indices[i], name);

    update();
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

void TreeView::setMaterial(const QList<unsigned int> &indexes, Material *m)
{
    auto indices = static_cast<TreeModel*>(model())->getIndices();

    for (unsigned int i : indexes)
        setMaterial(indices[i], m);

    update();
}

void TreeView::dumpModel(const QModelIndex& root, QList<unsigned int>& id, std::function<bool(const QModelIndex&)> functor) const
{
    bool ok;
    unsigned int i = root.sibling(root.row(), 2).data().toUInt(&ok);

    if (ok)
        id.append(i);

    for (int r = 0; r < model()->rowCount(root); r++)
    {
        auto c = root.child(r, 0);

        if (functor(c))
            dumpModel(c, id, functor);
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
    dumpModel(root.sibling(root.row(), 0), id, [=](const QModelIndex& c) { return getVisibility(c, m) == Default; });

    auto root_ = root;

    while (getVisibility(root_, m) == Default)
        root_ = root_.parent();

    if (m == Editor)
        scene->customSelection(id).setVisible_(getVisibility(root_, m) == On);
}

Material* TreeView::getMaterial(const QModelIndex &root) const
{
    if (root.isValid())
    {
        auto list = model()->data(root.sibling(root.row(), 5)).toList();
        if (list.isEmpty())
            return nullptr;
        else
            return qobject_cast<Material*>(list.last().value<QObject*>());
    }
    else
        return nullptr;
}

void TreeView::setMaterial(const QModelIndex &root, Material *m)
{
    auto index = root.sibling(root.row(), 5);

    auto list = index.data().toList();
    list.append(QVariant::fromValue(m));

    model()->setData(index, list);

    QList<unsigned int> id;
    dumpModel(root.sibling(root.row(), 0), id, [=](const QModelIndex& c) { return getMaterial(c) == nullptr; });

    scene->customSelection(id).setColor(m->getColor());
}

#include <QMouseEvent>

void TreeView::mousePressEvent(QMouseEvent *event)
{
    hv->mousePressEvent(event);

    if (viewport()->cursor().shape() == Qt::SplitHCursor)
        state = ResizeSection;
    else
    {
        auto index = indexAt(event->pos());

        switch (index.column())
        {
        case 3:
            state = ChangeVisibility;
            vm = event->pos().y() < visualRect(index).center().y() + 3 ? Editor : Renderer;
            cv = Visibility((getVisibility(index, vm) + 1) % 3);

            setVisibility(index, cv, vm);

            if (selectionModel()->isSelected(index))
                emit visibilityChanged(vm);

            update();
            break;
        case 5:
            update();
            break;
        default:
            if (!index.isValid())
                clearSelection();

            QTreeView::mousePressEvent(event);
        }
    }
}

void TreeView::mouseMoveEvent(QMouseEvent *event)
{
    switch (state)
    {
    case NoState:
        QTreeView::mouseMoveEvent(event);

    case ResizeSection:
        hv->mouseMoveEvent(event);

        if (hv->cursor().shape() != viewport()->cursor().shape())
        {
            if (viewport()->testAttribute(Qt::WA_SetCursor))
                viewport()->unsetCursor();
            else
                viewport()->setCursor(hv->cursor());
        }

        break;

    case ChangeVisibility:
        if (!testAttribute(Qt::WA_SetCursor))
            setCursor(Qt::DragCopyCursor);

        auto index = indexAt(event->pos());

        setVisibility(index, cv, vm);

        if (selectionModel()->isSelected(index))
            emit visibilityChanged(vm);

        update();
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
        hv->mouseReleaseEvent(event);
        state = NoState;
        break;

    case ChangeVisibility:
        state = NoState;
        unsetCursor();
        break;
    }
}

void TreeView::paintEvent(QPaintEvent *event)
{
    QTreeView::paintEvent(event);

    QPainter p(viewport());
    p.setPen("#2d2d2d");

    int x;
    x = hv->sectionViewportPosition(3);
    p.drawLine(QPoint(x, 0), QPoint(x, height()));
    x = hv->sectionViewportPosition(5);
    p.drawLine(QPoint(x, 0), QPoint(x, height()));
}

void TreeView::selectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
    QTreeView::selectionChanged(selected, deselected);

    for (auto i : selected.indexes())
    {
        auto j = i.parent();
        int c;

        do
        {
            c = j.data(Qt::UserRole).toInt();
            model()->setData(j, c + 1, Qt::UserRole);
        } while (c == 0 && (j = j.parent()).isValid());
    }

    for (auto i : deselected.indexes())
    {
        auto j = i.parent();
        int c;

        do
        {
            c = j.data(Qt::UserRole).toInt();
            model()->setData(j, c - 1, Qt::UserRole);
        } while (c == 1 && (j = j.parent()).isValid());
    }

    update();
}

void TreeView::dragEnterEvent(QDragEnterEvent *event)
{
    if (qobject_cast<Material*>(event->source()))
        event->acceptProposedAction();
}

void TreeView::dragMoveEvent(QDragMoveEvent *event)
{
    auto index = indexAt(event->pos());

    if (index.isValid() && index.column() != 5)
        event->acceptProposedAction();
    else
        event->ignore();
}

void TreeView::dropEvent(QDropEvent *event)
{
    event->acceptProposedAction();
    setMaterial(indexAt(event->pos()), qobject_cast<Material*>(event->source()));
    update();
}
