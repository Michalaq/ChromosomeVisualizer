#include "legend.h"

Legend::Legend(const QString & text, const QColor& color, QWidget *parent) :
    QLabel(text, parent),
    color(color),
    undergraph("Show undergraph", this),
    visible("Visible", this)
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
    return undergraph.isChecked() && visible.isChecked() ? QColor(color.red(), color.green(), color.blue(), 0x80) : Qt::transparent;
}

#include <QPainter>

void Legend::paintEvent(QPaintEvent *event)
{
    QLabel::paintEvent(event);

    QPainter painter(this);

    painter.setRenderHint(QPainter::Antialiasing);

    painter.setPen(QPen(color, 2.));
    painter.setBrush(undergraph.isChecked() ? QBrush(color) : Qt::NoBrush);

    painter.drawEllipse(8, 8, 9, 9);
}

#include <QMouseEvent>
#include <QColorDialog>
#include <QMenu>

void Legend::mousePressEvent(QMouseEvent *event)
{
    QLabel::mousePressEvent(event);

    if (event->button() == Qt::LeftButton)
    {
        QColor tmp = QColorDialog::getColor(color, this);

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
