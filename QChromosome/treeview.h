#ifndef TREEVIEW_H
#define TREEVIEW_H

#include <QTreeView>
#include "treemodel.h"
#include "../QtChromosomeViz_v2/VizWidget.hpp"

class VizWidget;

class TreeView : public QTreeView
{
    Q_OBJECT
public:
    explicit TreeView(QWidget *parent = 0);
    ~TreeView();

    void setSelection(const QList<unsigned int>& indexes);
    void setScene(VizWidget* s);

    Visibility getVisibility(const QList<unsigned int>& indexes) const;
    void setVisibility(const QList<unsigned int>& indexes, Visibility v);

signals:
    void vieChanged();

public slots:

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

private:
    Visibility cv;
    QMap<QModelIndex, bool> vie;

    VizWidget* scene;

    void dumpModel(const QModelIndex& root, QList<unsigned int>& id);

    Visibility getVisibility(const QModelIndex& root) const;
    void setVisibility(const QModelIndex& root, Visibility v);

    enum TreeViewState
    {
        NoState,
        ResizeSection,
        ChangeVisibility
    } state;
    int cw, cp;
};

#endif // TREEVIEW_H
