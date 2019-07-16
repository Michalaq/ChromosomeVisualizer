#include "legend.h"
#include <QLineSeries>

Legend::Legend(QtCharts::QAbstractSeries *s, QWidget *parent) :
    QLabel(s->name(), parent),
    series(s),
    hover(false)
{
    connect(series, &QtCharts::QAbstractSeries::nameChanged, [this]{setText(series->name());});
}

Legend::~Legend()
{

}

QColor Legend::pen() const
{
    return series->isVisible() ? reinterpret_cast<QtCharts::QLineSeries*>(series)->color() : Qt::transparent;
}

QColor Legend::brush() const
{
    QColor ans = reinterpret_cast<QtCharts::QLineSeries*>(series)->color();
    ans.setAlphaF(reinterpret_cast<QtCharts::QLineSeries*>(series)->opacity());

    return series->isVisible() ? ans : Qt::transparent;
}

#include <QPainter>

void Legend::paintEvent(QPaintEvent *event)
{
    QLabel::paintEvent(event);

    if (!series->isVisible())
        return;

    QPainter painter(this);

    painter.setRenderHint(QPainter::Antialiasing);

    painter.setPen(pen());
    painter.setBrush(brush());

    painter.drawEllipse(8, 8, 9, 9);

    if (hover)
    {
        painter.setPen(Qt::NoPen);
        painter.setBrush(QBrush(pen()));
        painter.setOpacity(.25);
        painter.drawEllipse(3, 3, 19, 19);
    }
}

void Legend::mousePressEvent(QMouseEvent *event)
{
    QLabel::mousePressEvent(event);

    emit selected();
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
