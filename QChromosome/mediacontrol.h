#ifndef MEDIACONTROL_H
#define MEDIACONTROL_H

#include <QPushButton>
#include <QSvgRenderer>

class MediaControl : public QPushButton
{
    Q_OBJECT
public:
    explicit MediaControl(QWidget *parent = 0);
    virtual ~MediaControl();

    void enterEvent(QEvent*);
    void leaveEvent(QEvent*);

    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

    void paintEvent(QPaintEvent *event);

    void load(const QString &file);

private:
    QSvgRenderer *icon;
};

#endif // MEDIACONTROL_H
