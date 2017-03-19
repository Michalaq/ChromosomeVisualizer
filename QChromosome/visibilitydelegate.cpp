#include "visibilitydelegate.h"

VisibilityDelegate::VisibilityDelegate(QObject *parent) : QItemDelegate(parent)
{

}

#include <QPainter>
#include <QApplication>

void VisibilityDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QApplication::style()->drawControl(QStyle::CE_ItemViewItem, &option, painter);
    painter->setBrush(QBrush(index.data().toString() == "On" ? Qt::green : Qt::red));
    painter->drawEllipse(option.rect);
}
