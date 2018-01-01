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

    void pick(QPersistentModelIndex object, bool spontaneous = true);
    void setIndexValidator(std::function<bool(const QPersistentModelIndex&)> iv);

signals:
    void picked(QPersistentModelIndex);

public slots:

protected:
    void mouseDoubleClickEvent(QMouseEvent *event);

private:
    static QSignalMapper sm;
    QPersistentModelIndex obj;
    QAction* icon;
    std::function<bool(const QPersistentModelIndex&)> indexValidator;
};

class Pickable
{
public:
    virtual QPersistentModelIndex pick(const QPoint& pos) = 0;
};

Q_DECLARE_INTERFACE(Pickable, "Pickable")

#endif // PICKWIDGET_H
