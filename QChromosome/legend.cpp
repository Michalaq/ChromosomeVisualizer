#include "legend.h"

Legend::Legend(const QString & text, const QColor& color, QWidget *parent) :
    QLabel(text, parent),
    color(color),
    undergraph("Show undergraph", this),
    visible("Visible", this),
    hover(false)
{
    undergraph.setCheckable(true);
    undergraph.setChecked(true);

    visible.setCheckable(true);
    visible.setChecked(true);
}

QColor Legend::pen() const
{
    return visible.isChecked() ? color : Qt::transparent;
}

QColor Legend::brush() const
{
    return undergraph.isChecked() && visible.isChecked() ? QColor(color.red(), color.green(), color.blue(), 0x10) : Qt::transparent;
}

#include <QPainter>

void Legend::paintEvent(QPaintEvent *event)
{
    QLabel::paintEvent(event);

    if (!visible.isChecked())
        return;

    QPainter painter(this);

    painter.setRenderHint(QPainter::Antialiasing);

    painter.setPen(QPen(color, 2.));
    painter.setBrush(undergraph.isChecked() ? QBrush(color) : Qt::NoBrush);

    painter.drawEllipse(8, 8, 9, 9);

    if (hover)
    {
        painter.setPen(Qt::NoPen);
        painter.setBrush(QBrush(color));
        painter.setOpacity(.25);
        painter.drawEllipse(3, 3, 19, 19);
    }
}

#include <QMouseEvent>
#include <QColorDialog>
#include <QMenu>

void Legend::mousePressEvent(QMouseEvent *event)
{
    QLabel::mousePressEvent(event);

    if (event->button() == Qt::LeftButton)
    {
        QColor tmp = QColorDialog::getColor(color);

        if (tmp.isValid())
        {
            color = tmp;

            update();
            emit changed();
        }
    }

    if (event->button() == Qt::RightButton)
    {
        if (QMenu::exec({ &undergraph, &visible }, event->globalPos()))
        {
            update();
            emit changed();
        }
    }
}

void Legend::enterEvent(QEvent *event)
{
    QLabel::enterEvent(event);

    hover = true;
    update();
}

void Legend::leaveEvent(QEvent *event)
{
    QLabel::leaveEvent(event);

    hover = false;
    update();
}
