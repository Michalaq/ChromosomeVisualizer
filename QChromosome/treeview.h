#ifndef TREEVIEW_H
#define TREEVIEW_H

#include <QTreeView>
#include "treemodel.h"
#include "../QtChromosomeViz_v2/VizWidget.hpp"

class TreeView : public QTreeView
{
    Q_OBJECT
public:
    explicit TreeView(QWidget *parent = 0);
    ~TreeView();

    void setSelection(const QList<unsigned int>& indexes);
    void setScene(VizWidget* s);
    void setVisibility(const QList<unsigned int>& indexes, Visibility v);

signals:

public slots:

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

private:
    int state;
    QMap<QModelIndex, bool> vie;
    VizWidget* scene;

    void dumpModel(const QModelIndex& root, QList<unsigned int>& id);
    void setVisibility(QModelIndex root, Visibility v);
};

#endif // TREEVIEW_H
