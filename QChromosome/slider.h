#ifndef SLIDER_H
#define SLIDER_H

#include "softslider.h"
#include "interpolator.h"

class Slider : public SoftSlider
{
    Q_OBJECT
public:
    explicit Slider(QWidget *parent = 0);

    QSize minimumSizeHint() const;

    void setInterpolator(Interpolator *_ip);

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

    void paintEvent(QPaintEvent *event);

signals:
    void keyframeSelected(int);

public slots:

private:
    Interpolator *ip;
    bool movemarker;
    int frame;
};

#endif // SLIDER_H
