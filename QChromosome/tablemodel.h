#ifndef TABLEMODEL_H
#define TABLEMODEL_H

#include <QAbstractTableModel>

class TableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit TableModel(const QStringList& h, QObject *parent = 0);
    ~TableModel();

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

    bool insertRows(int row, int count, const QModelIndex &parent) override;
    bool removeRows(int row, int count, const QModelIndex &parent) override;

private:
    QStringList header;
    QVector<QVector<QVariant>> database;
    QVector<QVariant> placeholder;

    QMap<QVariant, QPersistentModelIndex> fst2ix;

signals:
    void foo(QModelIndex ix, QVariant old);//TODO hack dla defaults, usunąć

public slots:
};

#endif // TABLEMODEL_H
