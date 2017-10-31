#include "namedelegate.h"

NameDelegate::NameDelegate(QObject *parent) : QStyledItemDelegate(parent)
{

}

NameDelegate::~NameDelegate()
{

}

#include <QLineEdit>

void NameDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    qobject_cast<QLineEdit*>(editor)->setText(index.data().toString());
}

#include "attributes.h"

void NameDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QString name = qobject_cast<QLineEdit*>(editor)->text();

    if (!name.isEmpty())
    {
        model->setData(index, name);

        if (index.sibling(index.row(), 1).data() == NodeType::AtomObject)
            qobject_cast<Attributes*>(parent())->updateName();
    }
}

void NameDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyleOptionViewItem opt = option;

    if (index.data(Qt::UserRole).toInt())
    {
        auto pal = option.palette;
        pal.setColor(QPalette::Text, Qt::lightGray);
        opt.palette = pal;
    }

    QStyledItemDelegate::paint(painter, opt, index);
}
