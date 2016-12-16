#ifndef SELECTION_H
#define SELECTION_H

#include <QWidget>

enum SelectionType {
    CUSTOM_SHAPE_SELECTION,
    RECTANGULAR_SELECTION
};

class Selection : public QWidget
{
    Q_OBJECT
public:
    explicit Selection(QWidget *parent = 0);
    ~Selection();

    void setSelectionType(SelectionType t);

protected:
    void paintEvent(QPaintEvent *event);

    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

private:
    QPoint tl, br;
    bool isSelecting, isSelectingState;
    QRect sr;
    QPainterPath path;
    SelectionType st;

signals:
    void selectionPathChanged(const QPainterPath&, Qt::KeyboardModifiers);

public slots:
    void enableSelecting(bool b);
};

#endif // SELECTION_H
