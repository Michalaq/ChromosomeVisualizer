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

    int i = index.data(Qt::UserRole + 1).toInt();

    if (i != -1)
    {
        painter->save();
        painter->setRenderHint(QPainter::Antialiasing, false);
        painter->setPen("#308fbf");
        painter->setBrush(QBrush("#80308fbf"));
        painter->drawRect(QRect(option.rect.left(), option.rect.top(), delta - 1, delta - 1).translated(i * delta, 0));
        painter->restore();
    }

    QRect rect(option.rect.left(), option.rect.top(), delta, delta);

    for (auto i : index.data().toList())
    {
        qobject_cast<Material*>(i.value<QObject*>())->paint(painter, rect);
        rect.translate(delta, 0);
    }
}
