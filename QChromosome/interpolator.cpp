#include "interpolator.h"

Interpolator::Interpolator(QObject *parent) : QObject(parent)
{
    _x.set_boundary(tk::spline::first_deriv, 0, tk::spline::first_deriv, 0, true);
    _y.set_boundary(tk::spline::first_deriv, 0, tk::spline::first_deriv, 0, true);
    _z.set_boundary(tk::spline::first_deriv, 0, tk::spline::first_deriv, 0, true);
    _h.set_boundary(tk::spline::first_deriv, 0, tk::spline::first_deriv, 0, true);
    _p.set_boundary(tk::spline::first_deriv, 0, tk::spline::first_deriv, 0, true);
    _b.set_boundary(tk::spline::first_deriv, 0, tk::spline::first_deriv, 0, true);

    frame = keyframes.end();
}

Interpolator::~Interpolator()
{

}

#include <QVector>

void Interpolator::recordKeyframe(int frame, QPair<QVector3D,QVector3D> value)
{
    keyframes[frame] = value;

    updateCurves();
}

void Interpolator::updateCurves()
{
    int n = keyframes.size();

    if (n < 2)
        return;

    std::vector<double> d = keyframes.keys().toVector().toStdVector(), __x(n), __y(n), __z(n), __h(n), __p(n), __b(n);

    int i = 0;

    for (auto f : keyframes.values())
    {
        __x[i] = f.first.x();
        __y[i] = f.first.y();
        __z[i] = f.first.z();
        __h[i] = f.second.x();
        __p[i] = f.second.y();
        __b[i] = f.second.z();

        i++;
    }

    _x.set_points(d, __x);
    _y.set_points(d, __y);
    _z.set_points(d, __z);
    _h.set_points(d, __h);
    _p.set_points(d, __p);
    _b.set_points(d, __b);

    emit interpolationChanged();
}
