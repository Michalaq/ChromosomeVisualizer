#ifndef MOUSECONTROL_H
#define MOUSECONTROL_H

#include "draggable.h"
#include <QSvgWidget>
#include <QGraphicsColorizeEffect>


class MouseControl : public Draggable
{
    Q_OBJECT

    Q_PROPERTY(QColor color READ color WRITE setColor)
    Q_PROPERTY(QString icon READ icon WRITE setIcon)

public:
    MouseControl(QWidget *parent = 0);
    virtual ~MouseControl();

    QColor color() const;
    void setColor(const QColor &color);

    QString icon() const;
    void setIcon(const QString &icon);

private:
    QSvgWidget *svg;
    QGraphicsColorizeEffect *effect;
};

#endif // CONTROL_H
