#ifndef CONTROL_H
#define CONTROL_H

#include "draggable.h"
#include <QSvgRenderer>


class Control : public Draggable
{
    Q_OBJECT

public:
    Control(QWidget *parent = 0);
    virtual ~Control();

    void paintEvent(QPaintEvent *event);

    void load(const QString &file);

private:
    QSvgRenderer *icon;
};

#endif // CONTROL_H
