#include "control.h"

Control::Control(QWidget* parent) : QSvgWidget(parent)
{
}

#include <QMouseEvent>

void Control::mousePressEvent(QMouseEvent *ev)
{
    QSvgWidget::mousePressEvent(ev);

    inp = ref = ev->globalPos();

    setCursor(Qt::BlankCursor);

    QPalette p(palette());
    p.setColor(QPalette::Background, QColor::fromCmykF(0.0, 0.0, 0.0, 0.7));
    setPalette(p);
}

#include <QScreen>
#include <QGuiApplication>

void Control::mouseMoveEvent(QMouseEvent *ev)
{
if (ev->globalPos() == ref) {ev->ignore(); return;}

QSvgWidget::mouseMoveEvent(ev);

    emit drag(ev->globalX() - ref.x(), ev->globalY() - ref.y());

    QPoint glob = QGuiApplication::primaryScreen()->geometry().center();
        QCursor::setPos(glob);
        ref = glob;
}

void Control::mouseReleaseEvent(QMouseEvent *ev)
{
    QSvgWidget::mouseReleaseEvent(ev);

    QCursor::setPos(inp);
    unsetCursor();

    QPalette p(palette());
    p.setColor(QPalette::Background, QColor::fromCmykF(0.0, 0.0, 0.0, 0.8));
    setPalette(p);
}

void Control::enterEvent(QEvent *ev)
{
    QSvgWidget::enterEvent(ev);

    setAutoFillBackground(true);
    QPalette p(palette());
    p.setColor(QPalette::Background, QColor::fromCmykF(0.0, 0.0, 0.0, 0.8));
    setPalette(p);
}

void Control::leaveEvent(QEvent *ev)
{
    QSvgWidget::leaveEvent(ev);

    setAutoFillBackground(false);
}

void Control::load(const QString &path)
{
        QFile file(path);
        file.open(QIODevice::ReadOnly | QIODevice::Text);
        QByteArray content = file.readAll();
        content.replace("#000000", QColor::fromCmykF(0.0, 0.0, 0.0, 0.2).name().toUtf8());

    QSvgWidget::load(content);
}
