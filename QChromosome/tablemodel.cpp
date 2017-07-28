#include "tablemodel.h"

TableModel::TableModel(QObject *parent) : QAbstractTableModel(parent)
{

}

TableModel::~TableModel()
{

}

Qt::ItemFlags TableModel::flags(const QModelIndex &index) const
{
    return QAbstractTableModel::flags(index) | (index.column() == 2 ? Qt::NoItemFlags : Qt::ItemIsEditable);
}

int TableModel::rowCount(const QModelIndex &parent) const
{
    return 5;
}

int TableModel::columnCount(const QModelIndex &parent) const
{
    return 3;
}

QVariant TableModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole)
        return "QVariant()";
    return QVariant();
}

QVariant TableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole)
        return "QVariant()";
    return QVariant();
}
