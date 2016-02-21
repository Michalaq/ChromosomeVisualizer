#ifndef MOUSECONTROL_H
#define MOUSECONTROL_H


#include "draggable.h"
#include <QIcon>
#include <QGraphicsColorizeEffect>

class MouseControl : public Draggable
{
    Q_OBJECT

    Q_PROPERTY(QIcon icon READ icon WRITE setIcon)
    Q_PROPERTY(QColor color READ color WRITE setColor)

public:
    MouseControl(QWidget *parent = 0);
    virtual ~MouseControl();

    QIcon icon() const;
    void setIcon(const QIcon &icon);

    QColor color() const;
    void setColor(const QColor &color);

protected:
    void paintEvent(QPaintEvent*);

private:
    QGraphicsColorizeEffect *effect;

    QIcon _icon;
};

#endif // CONTROL_H
