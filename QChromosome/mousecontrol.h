#ifndef MOUSECONTROL_H
#define MOUSECONTROL_H

#include "draggable.h"
#include <QSvgWidget>
#include <QGraphicsColorizeEffect>


class MouseControl : public Draggable
{
    Q_OBJECT

    Q_PROPERTY(QColor color READ getColor WRITE setColor)

public:
    MouseControl(QWidget *parent = 0);
    virtual ~MouseControl();

    QColor getColor() const;
    void setColor(const QColor &c);

    void setIcon(const QString &file);

private:
    QSvgWidget *icon;
    QGraphicsColorizeEffect *effect;
};

#endif // CONTROL_H
