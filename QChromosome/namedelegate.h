#ifndef NAMEDELEGATE_H
#define NAMEDELEGATE_H

#include <QStyledItemDelegate>

class NameDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit NameDelegate(QObject *parent = 0);

    void setEditorData(QWidget * editor, const QModelIndex & index) const;
    void setModelData(QWidget * editor, QAbstractItemModel * model, const QModelIndex & index) const;
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;

signals:

public slots:
};

#endif // NAMEDELEGATE_H
