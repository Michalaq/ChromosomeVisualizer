#include "tablemodel.h"

TableModel::TableModel(const QStringList &h, QObject *parent) : header(h), QAbstractTableModel(parent)
{
    //TODO usunąć
    database.push_back({"0", "LAM", QVariantList()});
    database.push_back({"1", "BIN", QVariantList()});
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
    return database.size();
}

int TableModel::columnCount(const QModelIndex &parent) const
{
    return header.size();
}

QVariant TableModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole)
        return database[index.row()][index.column()];

    if (role == Qt::UserRole + 1)
        return -1;

    return QVariant();
}

QVariant TableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    return role == Qt::DisplayRole ? header[section] : QVariant();
}
