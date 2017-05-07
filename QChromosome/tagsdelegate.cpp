#include "tagsdelegate.h"

TagsDelegate::TagsDelegate(QObject *parent) : QStyledItemDelegate(parent)
{

}

#include "material.h"
#include <QPainter>

void TagsDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    option.widget->style()->drawControl(QStyle::CE_ItemViewItem, &option, painter, option.widget);

    int delta = option.rect.height();

    QRect rect(option.rect.left(), option.rect.top(), delta - 2, delta - 2);

    for (auto i : index.data().toList())
    {
        qobject_cast<Material*>(i.value<QObject*>())->paint(painter, rect);
        rect.translate(delta, 0);
    }

    int i = index.data(Qt::UserRole + 1).toInt();

    if (i != -1)
    {
        painter->save();
        painter->setRenderHint(QPainter::Antialiasing, false);
        painter->setPen("#308fbf");
        painter->setBrush(Qt::NoBrush);
        painter->drawRect(QRect(option.rect.left(), option.rect.top(), delta - 3, delta - 3).translated(i * delta, 0));
        painter->restore();
    }
}
