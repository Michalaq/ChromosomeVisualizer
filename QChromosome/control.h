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

    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

    void paintEvent(QPaintEvent *event);

    void load(const QString &file);

private:
    QSvgRenderer *icon;

    bool clicked;
};

#endif // CONTROL_H
