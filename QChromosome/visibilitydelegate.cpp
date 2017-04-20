#include "visibilitydelegate.h"

VisibilityDelegate::VisibilityDelegate(QObject *parent) : QStyledItemDelegate(parent)
{

}

#include <QPainter>
#include <QApplication>

void VisibilityDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    option.widget->style()->drawControl(QStyle::CE_ItemViewItem, &option, painter, option.widget);

    painter->setRenderHint(QPainter::Antialiasing);
    painter->setPen(Qt::NoPen);

    painter->setBrush(index.data().toInt() == 0 ? Qt::gray : index.data().toInt() == 1 ? Qt::green : Qt::red);
    painter->drawEllipse(option.rect.center() - QPoint(0, 4), 3, 3);

    painter->setBrush(true ? Qt::gray : index.data().toInt() == 1 ? Qt::green : Qt::red);
    painter->drawEllipse(option.rect.center() + QPoint(0, 4), 3, 3);
}
