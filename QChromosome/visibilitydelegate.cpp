#include "visibilitydelegate.h"

VisibilityDelegate::VisibilityDelegate(QObject *parent) : QStyledItemDelegate(parent)
{

}

VisibilityDelegate::~VisibilityDelegate()
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
    painter->setPen(Qt::NoPen);
    painter->setBrush(QColor(v == Default ? "#767676" : v == On ? "#6df1af" : "#e65b5b"));
    painter->drawEllipse(QPointF(option.rect.center() - QPoint(0, 4)), 2.5, 2.5);

    v = Visibility(index.sibling(index.row(), index.column() + 1).data().toInt());
    painter->setPen(Qt::NoPen);
    painter->setBrush(QColor(v == Default ? "#767676" : v == On ? "#6df1af" : "#e65b5b"));
    painter->drawEllipse(QPointF(option.rect.center() + QPoint(0, 4)), 2.5, 2.5);
}
