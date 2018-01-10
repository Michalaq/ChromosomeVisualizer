#ifndef PICKWIDGET_H
#define PICKWIDGET_H

#include "lineedit.h"
#include <QSignalMapper>
#include <QModelIndex>
#include <QWidgetAction>
#include <functional>

class PickWidget : public LineEdit
{
    Q_OBJECT
public:
    explicit PickWidget(QWidget *parent = nullptr);

    static const QSignalMapper& getSignalMapper();

    void pick(const QPersistentModelIndex& object, bool spontaneous = true);

signals:
    void picked(const QPersistentModelIndex&);
    void selected(const QPersistentModelIndex&);

public slots:

protected:
    void mouseDoubleClickEvent(QMouseEvent *event);

private:
    static QSignalMapper sm;
    QPersistentModelIndex obj;
    QAction* icon;
};

class Pickable
{
public:
    virtual QPersistentModelIndex pick(const QPoint& pos) = 0;
};

Q_DECLARE_INTERFACE(Pickable, "Pickable")

#endif // PICKWIDGET_H
