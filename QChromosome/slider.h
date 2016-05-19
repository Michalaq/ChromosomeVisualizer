#ifndef SLIDER_H
#define SLIDER_H

#include "softslider.h"

class Slider : public SoftSlider
{
    Q_OBJECT
public:
    explicit Slider(QWidget *parent = 0);

    QSize minimumSizeHint() const;

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

    void paintEvent(QPaintEvent *event);

signals:

public slots:
};

#endif // SLIDER_H
