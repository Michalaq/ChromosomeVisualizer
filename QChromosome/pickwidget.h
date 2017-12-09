#ifndef PICKWIDGET_H
#define PICKWIDGET_H

#include "lineedit.h"
#include <QSignalMapper>
#include <QModelIndex>

class PickWidget : public LineEdit
{
    Q_OBJECT
public:
    explicit PickWidget(QWidget *parent = nullptr);

    static const QSignalMapper& getSignalMapper();

    void pick(QPersistentModelIndex object);

signals:
    void picked();

public slots:

protected:
    void mousePressEvent(QMouseEvent *event);

private:
    static QSignalMapper sm;
    QPersistentModelIndex obj;
};

class Pickable
{
public:
    virtual QPersistentModelIndex pick(const QPoint& pos) = 0;
};

Q_DECLARE_INTERFACE(Pickable, "Pickable")

#endif // PICKWIDGET_H
