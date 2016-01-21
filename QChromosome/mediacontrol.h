#ifndef MEDIACONTROL_H
#define MEDIACONTROL_H

#include <QPushButton>
#include <QGraphicsColorizeEffect>

class MediaControl : public QPushButton
{
    Q_OBJECT
public:
    explicit MediaControl(QWidget *parent = 0);
    virtual ~MediaControl();

    void enterEvent(QEvent*);
    void leaveEvent(QEvent*);

    void paintEvent(QPaintEvent *event);

private:
    QGraphicsColorizeEffect *effect;
};

#endif // MEDIACONTROL_H
