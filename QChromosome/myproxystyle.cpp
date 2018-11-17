#include "myproxystyle.h"

#include <QPainter>

void MyProxyStyle::drawItemText(QPainter *painter, const QRect &rect, int flags, const QPalette &pal, bool enabled, const QString &text, QPalette::ColorRole textRole) const
{
    QRect _rect(QPoint(), rect.size().transposed());

    painter->save();

    painter->translate(rect.center());
    painter->rotate(90);
    painter->translate(-_rect.center());

    QProxyStyle::drawItemText(painter, _rect, Qt::AlignLeft | (flags & ~Qt::AlignHorizontal_Mask), pal, enabled, text, textRole);

    painter->restore();
}
