#include "treeview.h"

TreeView::TreeView(QWidget *parent) :
    QTreeView(parent),
    state(NoState)
{
    setHeader(hv = new HeaderView(header()->orientation(), this));
    hv->setStretchLastSection(true);
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

void TreeView::dumpModel(const QModelIndex& root, QList<unsigned int>& id, std::function<bool(const QModelIndex&)> functor) const
{
    bool ok;
    unsigned int i = root.sibling(root.row(), 2).data().toUInt(&ok);

    if (ok)
        id.append(i);

    for (int r = 0; r < model()->rowCount(root); r++)
    {
        auto c = model()->index(r, 0, root);

        if (functor(c))
            dumpModel(c, id, functor);
    }
}

Material* getMaterial(const QModelIndex &root)
{
    if (root.isValid())
    {
        auto list = root.sibling(root.row(), 5).data().toList();

        return list.isEmpty() ? nullptr : qobject_cast<Material*>(list.last().value<QObject*>());
    }
    else
        return Material::getDefault();
}

Material* TreeView::takeSelectedMaterial()
{
    auto ans = qobject_cast<TreeModel*>(model())->removeMaterial(selectedTag, selectedTag.data(Qt::UserRole + 1).toInt());

    model()->setData(selectedTag, -1, Qt::UserRole + 1);
    selectedTag = QModelIndex();

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
            if (event->pos().x() < visualRect(index).center().x() + 4)
            {
                state = ChangeVisibility;
                vm = event->pos().y() < visualRect(index).center().y() + 3 ? Editor : Renderer;
                cv = Visibility((index.sibling(index.row(), vm).data().toInt() + 1) % 3);

                qobject_cast<TreeModel*>(model())->setVisibility(index, cv, vm);
            }
            else
            {
                if (index.sibling(index.row(), 1).data().toInt() == CameraObject)
                {
                    state = SetCamera;

                    auto m = static_cast<TreeModel*>(model());

                    if (index.sibling(index.row(), 6).data().toBool())
                        m->setCurrentCamera(QModelIndex());
                    else
                        m->setCurrentCamera(index.sibling(index.row(), 6));

                    emit cameraChanged(m->getCurrentCamera());
                }
            }

            update();
            break;
        case 5:
            state = DragTag;

            n = (event->x() - visualRect(index).x()) / 20;

            model()->setData(selectedTag, -1, Qt::UserRole + 1);
            selectedTag = QModelIndex();

            if (n < index.data().toList().length())
            {
                clicked = true;
                model()->setData(selectedTag = index, n, Qt::UserRole + 1);
            }
            else
            {
                model()->setData(selectedTag, -1, Qt::UserRole + 1);
                selectedTag = QModelIndex();

                clearSelection();
            }

            update();
            break;
        default:
            if (!index.isValid())
            {
                model()->setData(selectedTag, -1, Qt::UserRole + 1);
                selectedTag = QModelIndex();

                clearSelection();
                update();
            }

            QTreeView::mousePressEvent(event);
        }
    }
}

#include <QDrag>
#include <QMimeData>

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

        if (index.isValid())
        {
            qobject_cast<TreeModel*>(model())->setVisibility(index, cv, vm);
            update();
        }

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

#include <QPainter>

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

void TreeView::keyPressEvent(QKeyEvent *event)
{
    QTreeView::keyPressEvent(event);

    if (event->key() == Qt::Key_Delete || event->key() == Qt::Key_Backspace)
    {
        for (auto i : selectionModel()->selectedRows())
            model()->removeRow(i.row(), i.parent());
    }
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

    qobject_cast<TreeModel*>(model())->setMaterial(index, mat, n);

    model()->setData(selectedTag, -1, Qt::UserRole + 1);
    model()->setData(selectedTag = index, n, Qt::UserRole + 1);

    update();
}

bool TreeView::event(QEvent *event)
{
    if (event->type() == QEvent::ShortcutOverride && ((QKeyEvent*)event)->key() == Qt::Key_Delete && selectedTag.isValid())
        takeSelectedMaterial();

    return QTreeView::event(event);
}

QPersistentModelIndex TreeView::pick(const QPoint& pos)
{
    auto ans = indexAt(pos);
    return ans.isValid() ? ans.sibling(ans.row(), 0) : ans;
}
