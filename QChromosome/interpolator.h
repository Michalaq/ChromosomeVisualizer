#ifndef INTERPOLATOR_H
#define INTERPOLATOR_H

#include <QObject>
#include <QMap>
#include <QPair>
#include <QVector3D>
#include "spline.h"

class Interpolator : public QObject
{
    Q_OBJECT
public:
    explicit Interpolator(QObject *parent = 0);
    ~Interpolator();

    void recordKeyframe(int frame, QPair<QVector3D,QVector3D> value);

    QMap<double, QPair<QVector3D,QVector3D>> keyframes;
    tk::spline _x, _y, _z, _h, _p, _b;

    void updateCurves();

signals:
    void interpolationChanged();

public slots:
};

#endif // INTERPOLATOR_H
