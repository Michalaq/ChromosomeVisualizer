#include "visibilitydelegate.h"

VisibilityDelegate::VisibilityDelegate(QObject *parent) : QStyledItemDelegate(parent)
{

}

#include <QPainter>
#include <QApplication>

void VisibilityDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    option.widget->style()->drawControl(QStyle::CE_ItemViewItem, &option, painter, option.widget);
    painter->setBrush(QBrush(index.data().toInt() == 0 ? Qt::gray : index.data().toInt() == 1 ? Qt::green : Qt::red));
    painter->drawEllipse(option.rect.center(), 5, 5);
}
