#ifndef TABLEVIEW_H
#define TABLEVIEW_H

#include <QTableView>
#include "material.h"

class TableView : public QTableView
{
    Q_OBJECT
public:
    explicit TableView(QWidget *parent = 0);
    ~TableView();

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dropEvent(QDropEvent *event);

    bool event(QEvent *event);

private:
    Material* takeSelectedMaterial();

    enum TableViewState
    {
        NoState,
        DragTag
    } state;

    QModelIndex selectedTag;

    bool clicked = false;

signals:

public slots:
};

#include <QStyledItemDelegate>

class TableNameDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit TableNameDelegate(QObject *parent = 0);

    void setEditorData(QWidget * editor, const QModelIndex & index) const;
    void setModelData(QWidget * editor, QAbstractItemModel * model, const QModelIndex & index) const;
};

class TableIntDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit TableIntDelegate(QObject *parent = 0);

    void setEditorData(QWidget * editor, const QModelIndex & index) const;
    void setModelData(QWidget * editor, QAbstractItemModel * model, const QModelIndex & index) const;
};

class TableVectDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit TableVectDelegate(QObject *parent = 0);

    void setEditorData(QWidget * editor, const QModelIndex & index) const;
    void setModelData(QWidget * editor, QAbstractItemModel * model, const QModelIndex & index) const;
};

#endif // TABLEVIEW_H
