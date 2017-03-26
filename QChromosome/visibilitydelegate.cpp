#include "visibilitydelegate.h"

VisibilityDelegate::VisibilityDelegate(QObject *parent) : QStyledItemDelegate(parent)
{

}

#include <QPainter>
#include <QApplication>

void VisibilityDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    option.widget->style()->drawControl(QStyle::CE_ItemViewItem, &option, painter, option.widget);
    painter->setBrush(QBrush(index.data().toString() == "On" ? Qt::green : Qt::red));
    painter->drawEllipse(option.rect);
}
