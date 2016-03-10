#ifndef RANGESLIDER_H
#define RANGESLIDER_H

#include <QSlider>

class RangeSlider : public QSlider
{
    Q_OBJECT
public:
    explicit RangeSlider(QWidget *parent = 0);

protected:
    void paintEvent(QPaintEvent *event);

signals:

public slots:
};

#endif // RANGESLIDER_H
