#ifndef KEYFRAME_H
#define KEYFRAME_H

#include <QWidget>

namespace Ui {
    class Keyframes;
}

class SplineInterpolator;

class Keyframes : public QWidget
{
    Q_OBJECT
public:
    explicit Keyframes(QWidget *parent = 0);
    ~Keyframes();

    void setSplineInterpolator(SplineInterpolator *si);

signals:

private:
    Ui::Keyframes *ui;
    SplineInterpolator *interpolator;
};

#endif // KEYFRAME_H
