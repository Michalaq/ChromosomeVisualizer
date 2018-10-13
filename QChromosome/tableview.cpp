#include "tableview.h"

TableView::TableView(QWidget *parent) : QTableView(parent), state(NoState)
{

}

TableView::~TableView()
{

}

Material* TableView::takeSelectedMaterial()
{
    auto list = selectedTag.data().toList();
    auto ans = (Material*)list.takeAt(selectedTag.data(Qt::UserRole + 1).toInt()).value<QObject*>();

    ans->assign(selectedTag, false);

    model()->setData(selectedTag, list, Qt::DisplayRole);
    model()->setData(selectedTag, -1, Qt::UserRole + 1);
    selectedTag = QModelIndex();

    update();

    return ans;
}

#include <QMouseEvent>

void TableView::mousePressEvent(QMouseEvent *event)
{
    auto index = indexAt(event->pos());

    if (strcmp(itemDelegate(index)->metaObject()->className(), "TagsDelegate") == 0)
    {
        state = DragTag;

        int n = (event->x() - visualRect(index).x()) / 20;

        if (n < index.data().toList().length())
        {
            clicked = true;
            model()->setData(selectedTag, -1, Qt::UserRole + 1);
            model()->setData(selectedTag = index, n, Qt::UserRole + 1);
        }
        else
        {
            model()->setData(selectedTag, -1, Qt::UserRole + 1);
            selectedTag = QModelIndex();

            clearSelection();
        }

        update();
    }
    else
    {
        if (!index.isValid())
        {
            model()->setData(selectedTag, -1, Qt::UserRole + 1);
            selectedTag = QModelIndex();
        }

        QTableView::mousePressEvent(event);
    }

    tagsFocused = columnAt(event->x()) == model()->columnCount() - 1;
}

#include <QDrag>
#include <QMimeData>

void TableView::mouseMoveEvent(QMouseEvent *event)
{
    if (state == DragTag)
    {
        if (clicked)
        {
            clicked = false;

            QDrag *drag = new QDrag(this);
            drag->setMimeData(new QMimeData);
            drag->exec(Qt::MoveAction);
        }
    }
    else
        QTableView::mouseMoveEvent(event);
}

void TableView::mouseReleaseEvent(QMouseEvent *event)
{
    if (state == DragTag)
        clicked = false;
    else
        QTableView::mouseReleaseEvent(event);

    state = NoState;
}

void TableView::dragEnterEvent(QDragEnterEvent *event)
{
    if (qobject_cast<Material*>(event->source()) || event->source() == this)
        event->acceptProposedAction();
}

void TableView::dragMoveEvent(QDragMoveEvent *event)
{
    auto index = indexAt(event->pos());

    if (index.isValid() && (index.column() != model()->columnCount() - 1 || event->source() == this))
        event->acceptProposedAction();
    else
        event->ignore();
}

void TableView::dropEvent(QDropEvent *event)
{
    event->acceptProposedAction();

    auto index = indexAt(event->pos());
    index = index.sibling(index.row(), model()->columnCount() - 1);

    auto mat = event->source() == this ? takeSelectedMaterial() : qobject_cast<Material*>(event->source());

    int n = (event->pos().x() - visualRect(index).x()) / 20;
    int l = index.data().toList().length();

    if (n < 0) n = 0;
    if (n > l || event->source() != this) n = l;

    auto list = index.data().toList();
    list.insert(n, QVariant::fromValue(mat));

    model()->setData(index, list);

    mat->assign(index);

    model()->setData(selectedTag = index, n, Qt::UserRole + 1);

    update();
}

bool TableView::event(QEvent *event)
{
    if (event->type() == QEvent::ShortcutOverride && ((QKeyEvent*)event)->key() == Qt::Key_Delete)
    {
        if (tagsFocused)
        {
            if (selectedTag.isValid())
                takeSelectedMaterial();
        }
        else
            for (auto i : selectionModel()->selectedRows())
                model()->removeRow(i.row());
    }

    return QTableView::event(event);
}

#include <QLineEdit>

TableNameDelegate::TableNameDelegate(QObject *parent) : QStyledItemDelegate(parent)
{

}

void TableNameDelegate::setEditorData(QWidget * editor, const QModelIndex & index) const
{
    qobject_cast<QLineEdit*>(editor)->setText(index.data().toString());
}

void TableNameDelegate::setModelData(QWidget * editor, QAbstractItemModel * model, const QModelIndex & index) const
{
    QString str = qobject_cast<QLineEdit*>(editor)->text();

    if (!str.isEmpty())
        model->setData(index, str);
}

TableIntDelegate::TableIntDelegate(QObject *parent) : QStyledItemDelegate(parent)
{

}

void TableIntDelegate::setEditorData(QWidget * editor, const QModelIndex & index) const
{
    qobject_cast<QLineEdit*>(editor)->setText(QString::number(index.data().toInt()));
}

void TableIntDelegate::setModelData(QWidget * editor, QAbstractItemModel * model, const QModelIndex & index) const
{
    QString str = qobject_cast<QLineEdit*>(editor)->text();

    bool ok;
    unsigned value = str.toUInt(&ok);

    if (ok)
        model->setData(index, value);
}

TableVectDelegate::TableVectDelegate(QObject *parent) : QStyledItemDelegate(parent)
{

}

void TableVectDelegate::setEditorData(QWidget * editor, const QModelIndex & index) const
{
    qobject_cast<QLineEdit*>(editor)->setText(index.data().toString());
}

#include <QJsonDocument>
#include <QJsonArray>

void TableVectDelegate::setModelData(QWidget * editor, QAbstractItemModel * model, const QModelIndex & index) const
{
    QString str = qobject_cast<QLineEdit*>(editor)->text();

    auto doc = QJsonDocument::fromJson(str.toUtf8());

    if (doc.isArray())
    {
        for (auto i : doc.array())
            if (!i.isDouble()) return;

        model->setData(index, doc.toJson(QJsonDocument::Compact));
    }
}
