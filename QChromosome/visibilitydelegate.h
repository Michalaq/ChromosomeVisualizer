#ifndef VISIBILITYDELEGATE_H
#define VISIBILITYDELEGATE_H

#include <QStyledItemDelegate>

class VisibilityDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit VisibilityDelegate(QObject *parent = 0);
    ~VisibilityDelegate();

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;

signals:

public slots:

private:
    QImage focus1, focus2;
};

#endif // VISIBILITYDELEGATE_H
