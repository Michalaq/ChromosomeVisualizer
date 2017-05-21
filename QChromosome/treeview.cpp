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
    setFocusPolicy(Qt::ClickFocus);
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
        return Material::getDefault();
}

void TreeView::setMaterial(const QModelIndex &root, Material *m, int pos)
{
    auto index = root.sibling(root.row(), 5);

    auto list = index.data().toList();
    list.insert(pos, QVariant::fromValue(m));

    model()->setData(index, list);

    if (pos >= list.length() - 1)
    {
        QList<unsigned int> id;
        dumpModel(root.sibling(root.row(), 0), id, [=](const QModelIndex& c) { return getMaterial(c) == nullptr; });

        scene->customSelection(id).setMaterial(m);
    }

    m->assign(index);
}

void TreeView::updateAttributes(const Material *m)
{
    QList<unsigned int> id;

    for (auto root : m->getAssigned())
        if (root.data().toList().last().value<QObject*>() == m)
            dumpModel(root.sibling(root.row(), 0), id, [=](const QModelIndex& c) { return getMaterial(c) == nullptr; });

    scene->customSelection(id).setMaterial(m);

    update();
}

Material* TreeView::takeSelectedMaterial()
{
    auto list = selectedTag.data().toList();
    auto ans = (Material*)list.takeAt(selectedTag.data(Qt::UserRole + 1).toInt()).value<QObject*>();

    ans->assign(selectedTag, false);

    model()->setData(selectedTag, list, Qt::DisplayRole);
    model()->setData(selectedTag, -1, Qt::UserRole + 1);

    update();

    QModelIndex root = selectedTag;

    while (!getMaterial(root))
        root = root.parent();

    QList<unsigned int> id;
    dumpModel(selectedTag.sibling(selectedTag.row(), 0), id, [=](const QModelIndex& c) { return getMaterial(c) == nullptr; });

    scene->customSelection(id).setMaterial(getMaterial(root));

    return ans;
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
        int n;

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
            state = DragTag;

            n = (event->x() - visualRect(index).x()) / 20;

            model()->setData(selectedTag, -1, Qt::UserRole + 1);

            if (n < index.data().toList().length())
            {
                clicked = true;
                model()->setData(selectedTag = index, n, Qt::UserRole + 1);
            }

            update();
            break;
        default:
            if (!index.isValid())
            {
                model()->setData(selectedTag, -1, Qt::UserRole + 1);
                clearSelection();
                update();
            }

            QTreeView::mousePressEvent(event);
        }
    }
}

void TreeView::mouseMoveEvent(QMouseEvent *event)
{
    auto index = indexAt(event->pos());

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

        setVisibility(index, cv, vm);

        if (selectionModel()->isSelected(index))
            emit visibilityChanged(vm);

        update();
        break;

    case DragTag:
        if (clicked)
        {
            clicked = false;

            QDrag *drag = new QDrag(this);
            drag->setMimeData(new QMimeData);
            drag->exec(Qt::MoveAction);
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
        hv->mouseReleaseEvent(event);
        break;

    case ChangeVisibility:
        unsetCursor();
        break;

    case DragTag:
        clicked = false;
        break;
    }

    state = NoState;
}

void TreeView::paintEvent(QPaintEvent *event)
{
    QTreeView::paintEvent(event);

    QPainter p(viewport());
    p.setPen("#2d2d2d");

    int x;
    x = hv->sectionViewportPosition(3);
    p.drawLine(QPoint(x, 0), QPoint(x, height()));
    x = hv->sectionViewportPosition(5) - 1;
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
    if (qobject_cast<Material*>(event->source()) || event->source() == this)
        event->acceptProposedAction();
}

void TreeView::dragMoveEvent(QDragMoveEvent *event)
{
    auto index = indexAt(event->pos());

    if (index.isValid() && (index.column() != 5 || event->source() == this))
        event->acceptProposedAction();
    else
        event->ignore();
}

void TreeView::dropEvent(QDropEvent *event)
{
    event->acceptProposedAction();

    auto index = indexAt(event->pos());
    index = index.sibling(index.row(), 5);

    auto mat = event->source() == this ? takeSelectedMaterial() : qobject_cast<Material*>(event->source());

    int n = (event->pos().x() - visualRect(index).x()) / 20;
    int l = index.data().toList().length();

    if (n < 0) n = 0;
    if (n > l || event->source() != this) n = l;

    setMaterial(index, mat, n);

    model()->setData(selectedTag, -1, Qt::UserRole + 1);
    model()->setData(selectedTag = index, n, Qt::UserRole + 1);

    update();
}

bool TreeView::event(QEvent *event)
{
    if (event->type() == QEvent::ShortcutOverride && ((QKeyEvent*)event)->key() == Qt::Key_Delete)
        takeSelectedMaterial();

    return QTreeView::event(event);
}
