#ifndef TABLEWIDGET_H
#define TABLEWIDGET_H

#include <QTableWidget>
#include "material.h"

class TableWidget : public QTableWidget
{
    Q_OBJECT
public:
    explicit TableWidget(QWidget *parent = 0);
    ~TableWidget();

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

#endif // TABLEWIDGET_H
