#include "camera.h"

const qreal Camera::distanceFactor = 0.025;
const qreal Camera::angleFactor = 0.05;
const qreal Camera::wheelFactor = 2.00;

#include <QtMath>

Camera::Camera(QWidget *parent)
    : Draggable(parent),
      eye(60, 30, 60),
      x(1, 0, 0),
      y(0, 1, 0),
      z(0, 0, 1),
      h(45), p(-20), b(0),
      focalLength(36),
      apertureWidth(36),
      origin(0, 0, 0),
      rotationType(RT_World),
      nearClipping(.1),
      farClipping(1000.)
{
    QQuaternion q = QQuaternion::fromEulerAngles(p, h, b);

    x = q.rotatedVector(x);
    y = q.rotatedVector(y);
    z = q.rotatedVector(z);

    updateModelView();

    updateAngles();
}

void Camera::resizeEvent(QResizeEvent *event)
{
    emit projectionChanged(updateProjection());

    Draggable::resizeEvent(event);
}

#include <QWheelEvent>

void Camera::wheelEvent(QWheelEvent *event)
{
    scale(wheelFactor * event->angleDelta().y(), wheelFactor * event->angleDelta().y());

    Draggable::wheelEvent(event);
}

#include <QMetaMethod>

void Camera::connectNotify(const QMetaMethod &signal)
{
    if (signal == QMetaMethod::fromSignal(&Camera::modelViewChanged))
        emit modelViewChanged(modelView);

    if (signal == QMetaMethod::fromSignal(&Camera::projectionChanged))
        emit projectionChanged(projection);
}

void Camera::setOrigin(const QVector3D &o)
{
    origin = o;
}

QVector3D Camera::position() const
{
    return eye;
}

QVector3D Camera::EulerAngles() const
{
    return QVector3D(h, p, b);
}

void Camera::setPosition(const QVector3D &p)
{
    eye = p;

    emit modelViewChanged(updateModelView());
}

QVector3D Camera::lookAt() const
{
    return eye - z;
}

qreal Camera::angle() const
{
    return horizontalAngle;
}

void Camera::move(int dx, int dy)
{
    const qreal scale = distanceFactor * qAbs(QVector3D::dotProduct(eye - origin, z)) / focalLength;

    move(scale * dx, -scale * dy, 0.);
}

void Camera::rotate(int dx, int dy)
{
    rotate(-angleFactor * dx, -angleFactor * dy, 0.);
}

void Camera::scale(int dx, int)
{
    move(0., 0., distanceFactor * dx);
}

void Camera::setAspectRatio(qreal ar)
{
    aspectRatio = ar;
    updateAngles();
}

void Camera::move(qreal dx, qreal dy, qreal dz)
{
    /* change to global coordinates */
    QVector3D delta = x * dx + y * dy + z * dz;

    /* update eye position */
    eye -= delta;

    /* update scene */
    emit modelViewChanged(updateModelView());
}

void Camera::setEulerAgnles(qreal h_, qreal p_, qreal b_)
{
    int cp = RotationType::RT_Camera;
    std::swap(rotationType, cp);
    rotate(h - h_, p - p_, b - b_);
    std::swap(rotationType, cp);
}

void Camera::setFocalLength(qreal fl)
{
    focalLength = fl;
    updateAngles();
}

void Camera::setApertureWidth(qreal aw)
{
    apertureWidth = aw;
    updateAngles();
}

void Camera::setFieldOfView(qreal fov)
{
    focalLength = apertureWidth / 2 / qTan(qDegreesToRadians(fov) / 2);
    updateAngles();
}

void Camera::setRotationType(int rt)
{
    rotationType = rt;
}

void Camera::setNearClipping(qreal nc)
{
    nearClipping = nc;

    emit projectionChanged(updateProjection());
}

void Camera::setFarClipping(qreal fc)
{
    farClipping = fc;

    emit projectionChanged(updateProjection());
}

void Camera::setLookAt(const QVector3D &target)
{
    auto f = target - eye;
    f.setZ(-f.z());
    setEulerAgnles(qRadiansToDegrees(qAtan2(-f.x(), f.z())), qRadiansToDegrees(qAtan2(f.y(), QVector2D(f.x(), f.z()).length())), 0);
}

void Camera::rotate(qreal dh, qreal dp, qreal db)
{
    /* update Euler angles */
    h -= dh;
    p -= dp;
    b -= db;

    /* reset direction vectors */
    x = QVector3D(1, 0, 0);
    y = QVector3D(0, 1, 0);
    z = QVector3D(0, 0, 1);

    QQuaternion q, dq;

    /* rotate local coordinates */
    q = QQuaternion::fromAxisAndAngle(y, h);
    dq = QQuaternion::fromAxisAndAngle(y, -dh);

    x = q.rotatedVector(x);
    z = q.rotatedVector(z);

    if (rotationType == RT_World)
        eye = origin + dq.rotatedVector(eye - origin);

    q = QQuaternion::fromAxisAndAngle(x, p);
    dq = QQuaternion::fromAxisAndAngle(x, -dp);

    y = q.rotatedVector(y);
    z = q.rotatedVector(z);

    if (rotationType == RT_World)
        eye = origin + dq.rotatedVector(eye - origin);

    q = QQuaternion::fromAxisAndAngle(z, b);
    dq = QQuaternion::fromAxisAndAngle(z, -db);

    x = q.rotatedVector(x);
    y = q.rotatedVector(y);

    if (rotationType == RT_World)
        eye = origin + dq.rotatedVector(eye - origin);

    emit modelViewChanged(updateModelView());
}

QMatrix4x4& Camera::updateModelView()
{
    modelView.setToIdentity();

    modelView.translate(eye);
    modelView.rotate(QQuaternion::fromAxes(x, y, z));

    return modelView = modelView.inverted();
}

QMatrix4x4& Camera::updateProjection()
{
    const qreal aspectRatio_ = (qreal)width() / height();

    projection.setToIdentity();

    if (aspectRatio_ < aspectRatio)
    {
        projection.rotate(-90, {0, 0, 1});
        projection.perspective(horizontalAngle, 1. / aspectRatio_, nearClipping, farClipping);
        projection.rotate(+90, {0, 0, 1});
    }
    else
    {
        projection.perspective(verticalAngle, aspectRatio_, nearClipping, farClipping);
    }

    return projection;
}

void Camera::updateAngles()
{
    qreal d = qSqrt(1. + aspectRatio * aspectRatio);

    qreal w = apertureWidth * aspectRatio / d;
    qreal h = apertureWidth / d;

    horizontalAngle = (qreal)2.f * qRadiansToDegrees(qAtan(w / 2 / focalLength));
    verticalAngle = (qreal)2.f * qRadiansToDegrees(qAtan(h / 2 / focalLength));

    emit projectionChanged(updateProjection());

}
// Field of View: (qreal)2.f * qRadiansToDegrees(qAtan(apertureWidth / 2 / focalLength))
