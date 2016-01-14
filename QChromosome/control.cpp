#include "control.h"

Control::Control(QWidget *parent) :
    Draggable(parent),
    icon(new QSvgRenderer(this)),
    clicked(false)
{
    setStyleSheet(":hover {background: #333333;} :pressed {background: #4d4d4d;}");
}

Control::~Control()
{

}

void Control::mousePressEvent(QMouseEvent *event)
{
    Draggable::mousePressEvent(event);

    clicked = true;
    update();
}

void Control::mouseReleaseEvent(QMouseEvent *event)
{
    Draggable::mouseReleaseEvent(event);

    clicked = false;
}

#include <QStylePainter>
#include <QStyleOption>
#include <QPainter>

void Control::paintEvent(QPaintEvent *event)
{
    Draggable::paintEvent(event);

    QStyleOption option;
    option.initFrom(this);

    if (clicked)
        option.state |= QStyle::State_Sunken;

    QStylePainter(this).drawPrimitive(QStyle::PE_Widget, option);

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
