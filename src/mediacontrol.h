#ifndef MEDIACONTROL_H
#define MEDIACONTROL_H

#include <QPushButton>

class MediaControl : public QPushButton
{
    Q_OBJECT

public:
    explicit MediaControl(QWidget *parent = 0);
    virtual ~MediaControl();

protected:
    void paintEvent(QPaintEvent *event);
};

#endif // MEDIACONTROL_H
