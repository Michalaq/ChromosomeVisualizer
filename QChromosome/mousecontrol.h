#ifndef MOUSECONTROL_H
#define MOUSECONTROL_H


#include "draggable.h"
#include <QIcon>
#include <QLabel>
#include <QGraphicsColorizeEffect>

class MouseControl : public Draggable
{
    Q_OBJECT

    Q_PROPERTY(QIcon icon READ icon WRITE setIcon)
    Q_PROPERTY(QColor color READ color WRITE setColor)
    Q_PROPERTY(QColor pressed READ pressed WRITE setPressed)

public:
    MouseControl(QWidget *parent = 0);
    virtual ~MouseControl();

    void resizeEvent(QResizeEvent *event);

    QIcon icon() const;
    void setIcon(const QIcon &icon);

    QColor color() const;
    void setColor(const QColor &color);

    /* cf. QTBUG-2982 */
    QColor pressed() const;
    void setPressed(const QColor &color);

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

    void paintEvent(QPaintEvent*);

private:
    QGraphicsColorizeEffect *effect;
    QLabel *label;

    QIcon __icon;
    QColor __color;
    QColor __pressed;
};

#endif // CONTROL_H
