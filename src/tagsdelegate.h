#ifndef TAGSDELEGATE_H
#define TAGSDELEGATE_H

#include <QStyledItemDelegate>

class TagsDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit TagsDelegate(QObject *parent = 0);
    ~TagsDelegate();

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;

signals:

public slots:
};

#endif // TAGSDELEGATE_H
