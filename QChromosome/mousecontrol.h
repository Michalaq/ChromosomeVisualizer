#ifndef MOUSECONTROL_H
#define MOUSECONTROL_H


#include "draggable.h"
#include <QGraphicsColorizeEffect>

class MouseControl : public Draggable
{
    Q_OBJECT

    Q_PROPERTY(QColor color READ color WRITE setColor)

public:
    MouseControl(QWidget *parent = 0);
    virtual ~MouseControl();

    QColor color() const;
    void setColor(const QColor &color);

private:
    QGraphicsColorizeEffect *effect;
};

#endif // CONTROL_H
