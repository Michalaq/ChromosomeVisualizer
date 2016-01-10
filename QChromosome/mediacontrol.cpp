#include "mediacontrol.h"

MediaControl::MediaControl(QWidget *parent) :
    QPushButton(parent),
    icon(new QSvgRenderer(this))
{

}

MediaControl::~MediaControl()
{

}

void MediaControl::enterEvent(QEvent *event)
{
    QPushButton::enterEvent(event);

    setStyleSheet("background: #333333");
}

void MediaControl::leaveEvent(QEvent *event)
{
    QPushButton::leaveEvent(event);

    setStyleSheet("background: none");
}

void MediaControl::mousePressEvent(QMouseEvent *event)
{
    QPushButton::mousePressEvent(event);

    setStyleSheet("background: #4d4d4d");
}

void MediaControl::mouseReleaseEvent(QMouseEvent *event)
{
    QPushButton::mouseReleaseEvent(event);

    setStyleSheet("background: #333333");
}

#include <QPainter>

void MediaControl::paintEvent(QPaintEvent *event)
{
    //QPushButton::paintEvent(event);

    QPainter painter(this);
    icon->render(&painter);
}

#include <QFile>
#include <QRegularExpression>

void MediaControl::load(const QString &file)
{
    QFile svg(file);
    svg.open(QIODevice::ReadOnly | QIODevice::Text);

    QString content(svg.readAll());
    content.replace(QRegularExpression("#[a-f0-9]{6}"), "#cccccc");

    icon->load(content.toUtf8());
}
