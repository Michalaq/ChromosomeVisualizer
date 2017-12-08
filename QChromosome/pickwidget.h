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

    void pick(QModelIndex object);

signals:
    void picked();

public slots:

protected:
    void mousePressEvent(QMouseEvent *event);

private:
    static QSignalMapper sm;
    QModelIndex obj;
};

class Pickable
{
public:
    virtual QModelIndex pick(QPoint pos) const = 0;
};

#endif // PICKWIDGET_H
