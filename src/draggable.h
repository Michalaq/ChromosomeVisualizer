#ifndef DRAGGABLE_H
#define DRAGGABLE_H

#include <QPushButton>

class Draggable : public QPushButton
{
    Q_OBJECT

public:
    explicit Draggable(QWidget *parent = 0);
    virtual ~Draggable();

    static Qt::MouseButton pressedButton();

protected:
    void mousePressEvent(QMouseEvent*);
    void mouseMoveEvent(QMouseEvent*);
    void mouseReleaseEvent(QMouseEvent*);

private:
    QPoint initial;
    QPoint center;
    bool ignore;

    static Qt::MouseButton pressed;

signals:
    void delta(int, int);

protected slots:
    void setScreenGeometry(QRect);
};

#endif // DRAGGABLE_H
