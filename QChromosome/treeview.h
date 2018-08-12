#ifndef TREEVIEW_H
#define TREEVIEW_H

#include <QTreeView>
#include <QHeaderView>
#include <functional>
#include "camera.h"
#include "treemodel.h"
#include "pickwidget.h"

class HeaderView;
class Simulation;

class TreeView : public QTreeView, public Pickable
{
    Q_OBJECT
    Q_INTERFACES(Pickable)
public:
    explicit TreeView(QWidget *parent = 0);
    ~TreeView();

    void setSelection(const QList<unsigned int>& indexes);

    QPersistentModelIndex pick(const QPoint &pos);

signals:
    void cameraChanged(Camera*);

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
