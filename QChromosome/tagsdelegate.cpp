#include "tagsdelegate.h"

TagsDelegate::TagsDelegate(QObject *parent) : QStyledItemDelegate(parent)
{

}

#include "material.h"

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
}
