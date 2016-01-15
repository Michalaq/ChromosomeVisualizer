#ifndef CONTROL_H
#define CONTROL_H

#include "draggable.h"
#include <QSvgWidget>
#include <QGraphicsColorizeEffect>


class Control : public Draggable
{
    Q_OBJECT

public:
    Control(QWidget *parent = 0);
    virtual ~Control();

    void load(const QString &file);

private:
    QSvgWidget *icon;
    QGraphicsColorizeEffect *effect;
};

#endif // CONTROL_H
