#ifndef TABLEWIDGET_H
#define TABLEWIDGET_H

#include <QTableWidget>

class TableWidget : public QTableWidget
{
    Q_OBJECT
public:
    explicit TableWidget(QWidget *parent = 0);

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dropEvent(QDropEvent *event);

    bool event(QEvent *event);

signals:

public slots:
};

#endif // TABLEWIDGET_H
