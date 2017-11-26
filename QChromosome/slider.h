#ifndef SLIDER_H
#define SLIDER_H

#include "softslider.h"
#include "interpolator.h"
#include <QShortcut>
#include <QSvgRenderer>

class Slider : public SoftSlider
{
    Q_OBJECT
public:
    explicit Slider(QWidget *parent = 0);
    ~Slider();

    QSize minimumSizeHint() const;

    void setInterpolator(Interpolator *_ip);

    void setSplineInterpolator(SplineInterpolator *si);

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

    void paintEvent(QPaintEvent *event);
    void keyPressEvent(QKeyEvent *event);

public slots:

private:
    SplineInterpolator *interpolator;
    Interpolator *ip;
    bool movemarker;
    QImage pin;
};

#endif // SLIDER_H
