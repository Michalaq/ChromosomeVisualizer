#include "visibilitydelegate.h"

VisibilityDelegate::VisibilityDelegate(QObject *parent) :
    QStyledItemDelegate(parent),
    focus1(QSize(16, 16), QImage::Format_ARGB32_Premultiplied),
    focus2(QSize(16, 16), QImage::Format_ARGB32_Premultiplied)
{
    focus1.fill("#767676");
    focus2.fill(Qt::white);

    QImage alpha = QImage(":/dialogs/focus").alphaChannel().scaled(16, 16);

    focus1.setAlphaChannel(alpha);
    focus2.setAlphaChannel(alpha);
}

VisibilityDelegate::~VisibilityDelegate()
{

}

#include <QPainter>
#include "treeitem.h"

void VisibilityDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    option.widget->style()->drawControl(QStyle::CE_ItemViewItem, &option, painter, option.widget);

    painter->setRenderHint(QPainter::Antialiasing);
    painter->setPen(Qt::NoPen);

    Visibility v;

    v = Visibility(index.data().toInt());
    painter->setBrush(QColor(v == Default ? "#767676" : v == On ? "#6df1af" : "#e65b5b"));
    painter->drawEllipse(QPointF(option.rect.center() - QPoint(0, 4)), 2.5, 2.5);

    v = Visibility(index.sibling(index.row(), index.column() + 1).data().toInt());
    painter->setBrush(QColor(v == Default ? "#767676" : v == On ? "#6df1af" : "#e65b5b"));
    painter->drawEllipse(QPointF(option.rect.center() + QPoint(0, 6)), 2.5, 2.5);

    if (index.sibling(index.row(), 1).data() == NodeType::CameraObject)
        painter->drawImage(option.rect.center() + QPoint(4, -7), index.sibling(index.row(), 6).data().toBool() ? focus2 : focus1);
}
