#include "namedelegate.h"

NameDelegate::NameDelegate(QObject *parent) : QStyledItemDelegate(parent)
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
        qobject_cast<Attributes*>(parent())->updateName();
    }
}
