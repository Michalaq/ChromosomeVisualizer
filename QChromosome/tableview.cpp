#include "tableview.h"

TableView::TableView(QWidget *parent) : QTableView(parent)
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

    update();

    return ans;
}

#include <QMouseEvent>

void TableView::mousePressEvent(QMouseEvent *event)
{
    QTableView::mousePressEvent(event);
}

void TableView::mouseMoveEvent(QMouseEvent *event)
{
    QTableView::mouseMoveEvent(event);
}

void TableView::mouseReleaseEvent(QMouseEvent *event)
{
    QTableView::mouseReleaseEvent(event);
}

void TableView::dragEnterEvent(QDragEnterEvent *event)
{
    if (qobject_cast<Material*>(event->source()) || event->source() == this)
        event->acceptProposedAction();
}

void TableView::dragMoveEvent(QDragMoveEvent *event)
{
    auto index = indexAt(event->pos());

    if (index.isValid() && (index.column() != 5 || event->source() == this))
        event->acceptProposedAction();
    else
        event->ignore();
}

void TableView::dropEvent(QDropEvent *event)
{
    event->acceptProposedAction();

    auto index = indexAt(event->pos());
    index = index.sibling(index.row(), 5);

    auto mat = event->source() == this ? takeSelectedMaterial() : qobject_cast<Material*>(event->source());

    int n = (event->pos().x() - visualRect(index).x()) / 20;
    int l = index.data().toList().length();

    if (n < 0) n = 0;
    if (n > l || event->source() != this) n = l;

    auto list = index.data().toList();
    list.insert(n, QVariant::fromValue(mat));

    model()->setData(index, list);

    mat->assign(index);

    model()->setData(selectedTag, -1, Qt::UserRole + 1);
    model()->setData(selectedTag = index, n, Qt::UserRole + 1);

    update();
}

bool TableView::event(QEvent *event)
{
    if (event->type() == QEvent::ShortcutOverride && ((QKeyEvent*)event)->key() == Qt::Key_Delete && selectedTag.isValid())
        takeSelectedMaterial();

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

void TableVectDelegate::setModelData(QWidget * editor, QAbstractItemModel * model, const QModelIndex & index) const
{

}
