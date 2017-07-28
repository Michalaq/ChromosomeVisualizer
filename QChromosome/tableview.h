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

    QModelIndex selectedTag;

signals:

public slots:
};

#endif // TABLEVIEW_H
