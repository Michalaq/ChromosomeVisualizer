#include "visibilitydelegate.h"

VisibilityDelegate::VisibilityDelegate(QObject *parent) : QStyledItemDelegate(parent)
{

}

#include <QPainter>
#include "treemodel.h"

void VisibilityDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    option.widget->style()->drawControl(QStyle::CE_ItemViewItem, &option, painter, option.widget);

    painter->setRenderHint(QPainter::Antialiasing);
    painter->setPen(Qt::NoPen);

    Visibility v;

    v = Visibility(index.data().toInt());
    painter->setPen(v == Off ? Qt::white : Qt::transparent);
    painter->setBrush(v == Default ? Qt::gray : v == On ? Qt::white : Qt::transparent);
    painter->drawEllipse(option.rect.center() - QPoint(0, 4), 3, 3);

    v = Visibility(index.sibling(index.row(), index.column() + 1).data().toInt());
    painter->setPen(v == Off ? Qt::white : Qt::transparent);
    painter->setBrush(v == Default ? Qt::gray : v == On ? Qt::white : Qt::transparent);
    painter->drawEllipse(option.rect.center() + QPoint(0, 4), 3, 3);
}
