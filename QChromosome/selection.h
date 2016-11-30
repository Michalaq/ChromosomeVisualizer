#ifndef SELECTION_H
#define SELECTION_H

#include <QWidget>

class Selection : public QWidget
{
    Q_OBJECT
public:
    explicit Selection(QWidget *parent = 0);
    ~Selection();

protected:
    void paintEvent(QPaintEvent *event);

    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

private:
    QPoint tl, br;
    bool isSelecting, isSelectingState;
    QRect sr;

signals:
    void selectionRectChanged(const QRect&, Qt::KeyboardModifiers);

public slots:
    void enableSelecting(bool b);
};

#endif // SELECTION_H
