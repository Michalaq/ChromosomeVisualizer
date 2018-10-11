#include "tablemodel.h"

TableModel::TableModel(const QStringList &h, QObject *parent) :
    QAbstractTableModel(parent),
    header(h)
{
    placeholder = QVector<QVariant>(header.size(), QVariant());
    placeholder.push_back(-1);
}

TableModel::~TableModel()
{

}

int TableModel::rowCount(const QModelIndex &) const
{
    return database.size();
}

int TableModel::columnCount(const QModelIndex &) const
{
    return header.size();
}

QVariant TableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role == Qt::DisplayRole)
        return database[index.row()][index.column()];

    if (role == Qt::UserRole + 1)
        return database[index.row()].last();

    return QVariant();
}

QVariant TableModel::headerData(int section, Qt::Orientation, int role) const
{
    return role == Qt::DisplayRole ? header[section] : QVariant();
}

bool TableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid())
        return false;

    if (role == Qt::UserRole + 1)
    {
        database[index.row()].last() = value;
        return true;
    }
    else
    {auto old = index.data();//TODO hack dla defaults, usunąć
        // first column is a primary key
        if (index.column() == 0)
        {
            if (!fst2ix.contains(value))
            {
                fst2ix.remove(index.data());
                fst2ix.insert(value, QPersistentModelIndex(index));
                database[index.row()][index.column()] = value;
            }
            else
                return false;
        }
        else
            database[index.row()][index.column()] = value;

        emit foo(index, old);//TODO hack dla defaults, usunąć
        emit dataChanged(index, index);
        return true;
    }
}

Qt::ItemFlags TableModel::flags(const QModelIndex &index) const
{
    return QAbstractTableModel::flags(index) | (index.column() == 2 ? Qt::NoItemFlags : Qt::ItemIsEditable);
}

bool TableModel::insertRows(int row, int count, const QModelIndex &parent)
{
    if (parent.isValid())
        row += parent.row();

    beginInsertRows(parent, row, row + count - 1);
    database.insert(row, count, placeholder);
    endInsertRows();

    return true;
}

bool TableModel::removeRows(int row, int count, const QModelIndex &parent)
{
    if (parent.isValid())
        row += parent.row();

    beginRemoveRows(parent, row, row + count - 1);
    database.erase(database.begin() + row, database.begin() + row + count);
    endRemoveRows();

    return true;
}
