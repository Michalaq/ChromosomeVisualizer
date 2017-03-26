#ifndef VISIBILITYDELEGATE_H
#define VISIBILITYDELEGATE_H

#include <QStyledItemDelegate>

class VisibilityDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit VisibilityDelegate(QObject *parent = 0);

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;

signals:

public slots:
};

#endif // VISIBILITYDELEGATE_H
