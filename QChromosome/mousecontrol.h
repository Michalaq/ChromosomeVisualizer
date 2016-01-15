#ifndef MOUSECONTROL_H
#define MOUSECONTROL_H

#include "draggable.h"
#include <QSvgWidget>
#include <QGraphicsColorizeEffect>


class MouseControl : public Draggable
{
    Q_OBJECT

public:
    MouseControl(QWidget *parent = 0);
    virtual ~MouseControl();

    void load(const QString &file);

private:
    QSvgWidget *icon;
    QGraphicsColorizeEffect *effect;
};

#endif // CONTROL_H
