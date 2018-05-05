#include "statusbar.h"

StatusBar::StatusBar(QWidget *parent) : QStatusBar(parent)
{

}

#include <QPainter>
#include <QStyleOption>

void StatusBar::paintEvent(QPaintEvent *event)
{
    QPainter p(this);
    QStyleOption opt;
    opt.initFrom(this);
    style()->drawPrimitive(QStyle::PE_PanelStatusBar, &opt, &p, this);

    p.setPen(palette().foreground().color());
    p.drawText(QRect(6, 0, width() - 12, height()), Qt::AlignLeading | Qt::AlignVCenter | Qt::TextSingleLine, permanent.isEmpty() ? currentMessage() : permanent);
}

void StatusBar::clearPermanentMessage()
{
    permanent.clear();
    update();
}

void StatusBar::showPermanentMessage(const QString &text)
{
    permanent = text;
    update();
}
