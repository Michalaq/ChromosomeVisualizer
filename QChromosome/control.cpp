#include "control.h"

Control::Control(QWidget *parent) :
    Draggable(parent),
    icon(new QSvgRenderer(this))
{

}

Control::~Control()
{

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
