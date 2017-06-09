#ifndef MATERIALBROWSER_H
#define MATERIALBROWSER_H

#include <QWidget>
#include <QSplitter>
#include <QListView>

#include "material.h"

namespace Ui {
class MaterialBrowser;
}

class ListView : public QListView
{
    Q_OBJECT
public:
    explicit ListView(QWidget *parent = 0);

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);

private:
    bool clicked;

signals:

public slots:
};

class MaterialBrowser : public QWidget
{
    Q_OBJECT

public:
    explicit MaterialBrowser(QWidget *parent = 0);
    ~MaterialBrowser();

protected:
    void paintEvent(QPaintEvent *event);

signals:
    void materialsSelected(const QList<Material*>&);

private:
    Ui::MaterialBrowser *ui;

    QSplitter *s;
    ListView *lv;
    QWidget * w;
};

#include <QAbstractListModel>

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
    QList<Material*> materials;
};

#include <QStyledItemDelegate>

class MaterialDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit MaterialDelegate(QObject *parent = 0);

    void setEditorData(QWidget * editor, const QModelIndex & index) const;
    void setModelData(QWidget * editor, QAbstractItemModel * model, const QModelIndex & index) const;
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;

signals:

public slots:
};

#endif // MATERIALBROWSER_H
