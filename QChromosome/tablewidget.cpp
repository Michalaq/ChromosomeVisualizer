#include "tablewidget.h"

TableWidget::TableWidget(QWidget *parent) : QTableWidget(parent)
{

}

TableWidget::~TableWidget()
{

}

Material* TableWidget::takeSelectedMaterial()
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

void TableWidget::mousePressEvent(QMouseEvent *event)
{
    QTableWidget::mousePressEvent(event);
}

void TableWidget::mouseMoveEvent(QMouseEvent *event)
{
    QTableWidget::mouseMoveEvent(event);
}

void TableWidget::mouseReleaseEvent(QMouseEvent *event)
{
    QTableWidget::mouseReleaseEvent(event);
}

void TableWidget::dragEnterEvent(QDragEnterEvent *event)
{
    if (qobject_cast<Material*>(event->source()) || event->source() == this)
        event->acceptProposedAction();
}

void TableWidget::dragMoveEvent(QDragMoveEvent *event)
{
    auto index = indexAt(event->pos());

    if (index.isValid() && (index.column() != 5 || event->source() == this))
        event->acceptProposedAction();
    else
        event->ignore();
}

void TableWidget::dropEvent(QDropEvent *event)
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

bool TableWidget::event(QEvent *event)
{
    if (event->type() == QEvent::ShortcutOverride && ((QKeyEvent*)event)->key() == Qt::Key_Delete)
        takeSelectedMaterial();

    return QTableWidget::event(event);
}
