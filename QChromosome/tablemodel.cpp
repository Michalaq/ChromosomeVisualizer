#include "tablemodel.h"

TableModel::TableModel(const QStringList &h, QObject *parent) : header(h), QAbstractTableModel(parent)
{
    //TODO usunąć
    database.push_back({0, "LAM", QVariantList(), -1});
    database.push_back({1, "BIN", QVariantList(), -1});
}

TableModel::~TableModel()
{

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
        return database[index.row()].last();

    return QVariant();
}

QVariant TableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    return role == Qt::DisplayRole ? header[section] : QVariant();
}

bool TableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (role == Qt::EditRole)
        database[index.row()][index.column()] = value;

    emit dataChanged(index, index);
    return true;
}

Qt::ItemFlags TableModel::flags(const QModelIndex &index) const
{
    return QAbstractTableModel::flags(index) | (index.column() == 2 ? Qt::NoItemFlags : Qt::ItemIsEditable);
}

bool TableModel::insertRows(int row, int count, const QModelIndex &parent)
{
    return true;
}

bool TableModel::removeRows(int row, int count, const QModelIndex &parent)
{
    row += parent.row();
    database.erase(database.begin() + row, database.begin() + row + count);
    return true;
}