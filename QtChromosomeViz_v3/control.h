#ifndef CONTROL_H
#define CONTROL_H

#include <QSvgWidget>

class Control : public QSvgWidget
{
    Q_OBJECT

public:
    Control(QWidget *parent = 0);

    void mousePressEvent(QMouseEvent *ev);
    void mouseMoveEvent(QMouseEvent *ev);
    void mouseReleaseEvent(QMouseEvent *ev);

    void enterEvent(QEvent *ev);
    void leaveEvent(QEvent *ev);

    void load(const QString& file);

private:
    QPoint inp;
    QPoint ref;

signals:
    void drag(int dx, int dy);
};

#endif // CONTROL_H
