#ifndef TREEVIEW_H
#define TREEVIEW_H

#include <QTreeView>
#include <QHeaderView>
#include "treemodel.h"

class VizWidget;

enum VisibilityMode
{
    Editor = 3,
    Renderer = 4
};

class HeaderView;

class TreeView : public QTreeView
{
    Q_OBJECT
public:
    explicit TreeView(QWidget *parent = 0);
    ~TreeView();

    void setSelection(const QList<unsigned int>& indexes);
    void setScene(VizWidget* s);

    QVariant getName(const QList<unsigned int>& indexes) const;
    void setName(const QList<unsigned int>& indexes, const QString& name);

    Visibility getVisibility(const QList<unsigned int>& indexes, VisibilityMode m) const;
    void setVisibility(const QList<unsigned int>& indexes, Visibility v, VisibilityMode m);

signals:
    void visibilityChanged(VisibilityMode);

public slots:

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void paintEvent(QPaintEvent *event);

    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dropEvent(QDropEvent *event);

private:
    Visibility cv;
    VisibilityMode vm;

    VizWidget* scene;

    void dumpModel(const QModelIndex& root, QList<unsigned int>& id, VisibilityMode m);

    Visibility getVisibility(const QModelIndex& root, VisibilityMode m) const;
    void setVisibility(const QModelIndex& root, Visibility v, VisibilityMode m);

    enum TreeViewState
    {
        NoState,
        ResizeSection,
        ChangeVisibility
    } state;

    HeaderView *hv;
};

class HeaderView : public QHeaderView
{
    Q_OBJECT
    friend class TreeView;
public:
    explicit HeaderView(Qt::Orientation orientation, QWidget *parent = 0);
};

#endif // TREEVIEW_H
