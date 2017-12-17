#ifndef TREEVIEW_H
#define TREEVIEW_H

#include <QTreeView>
#include <QHeaderView>
#include "treemodel.h"
#include "material.h"
#include <functional>

class HeaderView;
class Simulation;

class TreeView : public QTreeView
{
    Q_OBJECT
public:
    explicit TreeView(QWidget *parent = 0);
    ~TreeView();

    void setSelection(const QList<unsigned int>& indexes);

    QVariant getName(const QList<unsigned int>& indexes) const;
    void setName(const QList<unsigned int>& indexes, const QString& name);

signals:
    void cameraChanged(Camera*);
    void visibilityChanged(VisibilityMode);

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void paintEvent(QPaintEvent *event);
    void keyPressEvent(QKeyEvent *event);

    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dropEvent(QDropEvent *event);

    bool event(QEvent *event);

    void selectionChanged(const QItemSelection &selected, const QItemSelection &deselected);

private:
    Visibility cv;
    VisibilityMode vm;

    void dumpModel(const QModelIndex& root, QList<unsigned int>& id, std::function<bool(const QModelIndex&)> functor) const;

    Material* takeSelectedMaterial();

    enum TreeViewState
    {
        NoState,
        ResizeSection,
        ChangeVisibility,
        DragTag,
        SetCamera
    } state;

    HeaderView *hv;

    QPersistentModelIndex selectedTag;

    bool clicked = false;
};

class HeaderView : public QHeaderView
{
    Q_OBJECT
    friend class TreeView;
public:
    explicit HeaderView(Qt::Orientation orientation, QWidget *parent = 0);
};

#endif // TREEVIEW_H
