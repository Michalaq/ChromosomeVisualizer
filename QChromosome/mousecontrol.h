#ifndef MOUSECONTROL_H
#define MOUSECONTROL_H

#include "draggable.h"

class MouseControl : public Draggable
{
    Q_OBJECT

public:
    explicit MouseControl(QWidget *parent = 0);
    virtual ~MouseControl();

protected:
    void paintEvent(QPaintEvent *event);
};

#endif // MOUSECONTROL_H
