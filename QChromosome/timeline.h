#ifndef TIMELINE_H
#define TIMELINE_H

#include "softslider.h"
#include "interpolator.h"

class Timeline : public SoftSlider
{
    Q_OBJECT
public:
    explicit Timeline(QWidget *parent = 0);
    ~Timeline();

    QSize minimumSizeHint() const;

    void setSplineInterpolator(SplineInterpolator *si);

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

    void paintEvent(QPaintEvent *event);
    void keyPressEvent(QKeyEvent *event);

private:
    SplineInterpolator *interpolator;
    bool movemarker;
    int lastValue;
    QImage pin;
};

#endif // TIMELINE_H
