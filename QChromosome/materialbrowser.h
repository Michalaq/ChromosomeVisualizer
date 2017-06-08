#ifndef MATERIALBROWSER_H
#define MATERIALBROWSER_H

#include <QWidget>
#include <QSplitter>
#include <QListView>

namespace Ui {
class MaterialBrowser;
}

class MaterialBrowser : public QWidget
{
    Q_OBJECT

public:
    explicit MaterialBrowser(QWidget *parent = 0);
    ~MaterialBrowser();

private:
    Ui::MaterialBrowser *ui;

    QSplitter *s;
    QListView *lv;
    QWidget * w;
};

#include <QAbstractListModel>
#include "material.h"

class MaterialListModel : public QAbstractListModel
{
    Q_OBJECT

public:
    MaterialListModel(QObject *parent = 0)
        : QAbstractListModel(parent) {}

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

    Qt::ItemFlags flags(const QModelIndex &index) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

    bool insertRows(int position, int rows, const QModelIndex &index = QModelIndex());
    bool removeRows(int position, int rows, const QModelIndex &index = QModelIndex());

private:
    QStringList names;
    QList<Material*> materials;
};

#include <QStyledItemDelegate>

class MaterialDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit MaterialDelegate(QObject *parent = 0);

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;

signals:

public slots:
};

#endif // MATERIALBROWSER_H
