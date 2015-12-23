#include "control.h"

Control::Control(QWidget *parent) : Draggable(parent), icon(new QSvgRenderer(this))
{

}

Control::~Control()
{

}

void Control::enterEvent(QEvent *event)
{
    Draggable::enterEvent(event);

    setStyleSheet("background: #333333");
}

void Control::leaveEvent(QEvent *event)
{
    Draggable::leaveEvent(event);

    setStyleSheet("background: none");
}

void Control::mousePressEvent(QMouseEvent *event)
{
    Draggable::mousePressEvent(event);

    setStyleSheet("background: #4d4d4d");
}

void Control::mouseReleaseEvent(QMouseEvent *event)
{
    Draggable::mouseReleaseEvent(event);

    setStyleSheet("background: #333333");
}

#include <QPainter>

void Control::paintEvent(QPaintEvent *event)
{
    Draggable::paintEvent(event);

    QPainter painter(this);
    icon->render(&painter);
}

#include <QFile>
#include <QRegularExpression>

void Control::load(const QString &file)
{
    QFile svg(file);
    svg.open(QIODevice::ReadOnly | QIODevice::Text);

    QString content(svg.readAll());
    content.replace(QRegularExpression("#[a-f0-9]{6}"), "#cccccc");

    icon->load(content.toUtf8());
}
