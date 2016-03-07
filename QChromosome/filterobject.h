#ifndef FILTEROBJECT_H
#define FILTEROBJECT_H

#include <QWidget>

#include <QVariant>
#include <QEvent>

class FilterObject : public QObject
{
    Q_OBJECT
public:
    explicit FilterObject(QObject *parent = 0);

protected:
    bool eventFilter(QObject *watched, QEvent *event);

private:
    /* cf. QTBUG-2982 */
    typedef QList<QPair<QByteArray, QVariant> > QPropertyStyleSheet;

    QHash<QPair<QObject*, QEvent::Type>, QPair<QPropertyStyleSheet, QPropertyStyleSheet*> > qproperty_enter;
    QHash<QPair<QObject*, QEvent::Type>, QPropertyStyleSheet> qproperty_leave;

    static const QHash<QString, QPair<QEvent::Type, QEvent::Type> > pseudostate2event;

    void cacheProperties(QWidget* widget, QHash<QString, QHash<QString, QHash<QString, QVariant> > > cache = {});

signals:

public slots:
};

#endif // FILTEROBJECT_H
