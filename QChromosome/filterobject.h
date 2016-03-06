#ifndef FILTEROBJECT_H
#define FILTEROBJECT_H

#include <QWidget>

class FilterObject : public QObject
{
    Q_OBJECT
public:
    explicit FilterObject(QWidget *parent = 0);

protected:
    bool eventFilter(QObject *watched, QEvent *event);

private:
    /* cf. QTBUG-2982 */
    void cacheProperties(QWidget* widget, QHash<QString, QHash<QString, QHash<QString, QVariant> > > cache = {});

signals:

public slots:
};

#endif // FILTEROBJECT_H
