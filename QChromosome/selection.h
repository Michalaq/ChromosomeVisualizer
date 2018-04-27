#ifndef SELECTION_H
#define SELECTION_H

#include <QOpenGLWidget>

enum SelectionType {
    NO_SELECTION,
    CUSTOM_SHAPE_SELECTION,
    RECTANGULAR_SELECTION
};

class Selection : public QOpenGLWidget
{
    Q_OBJECT
public:
    explicit Selection(QWidget *parent = 0);
    ~Selection();

    static void setSelectionType(SelectionType t);
    const QPainterPath& getSelectionPath() const;

protected:
    void paintEvent(QPaintEvent *event);

    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

private:
    QPoint tl, br;
    bool isSelecting;
    QRect sr;
    QPainterPath path;
    static SelectionType st;
};

#endif // SELECTION_H
