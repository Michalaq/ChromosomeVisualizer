#include "camera.h"

namespace RenderSettings
{
    const qreal aspectRatio = 320. / 240.;
}

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
      modifier(Qt::Key_unknown)
{
    QQuaternion q = QQuaternion::fromEulerAngles(p, h, b);

    x = q.rotatedVector(x);
    y = q.rotatedVector(y);
    z = q.rotatedVector(z);

    updateModelView();

    updateAngles();

    setFocus();
}

#include <QKeyEvent>

void Camera::keyPressEvent(QKeyEvent *event)
{
    if (event->isAutoRepeat())
        return event->ignore();

    if (modifier == Qt::Key_unknown)
    {
        switch (event->key())
        {
        case Qt::Key_M:
            modifier = Qt::Key_M;
            connect(this, SIGNAL(delta(int,int)), this, SLOT(move(int,int)));
            break;

        case Qt::Key_R:
            modifier = Qt::Key_R;
            connect(this, SIGNAL(delta(int,int)), this, SLOT(rotate(int,int)));
            break;

        case Qt::Key_S:
            modifier = Qt::Key_S;
            connect(this, SIGNAL(delta(int,int)), this, SLOT(scale(int,int)));
            break;
        }
    }

    Draggable::keyPressEvent(event);
}

void Camera::keyReleaseEvent(QKeyEvent *event)
{
    if (event->isAutoRepeat())
        return event->ignore();

    if (modifier == event->key())
    {
        modifier = Qt::Key_unknown;
        disconnect();
    }

    Draggable::keyReleaseEvent(event);
}

void Camera::resizeEvent(QResizeEvent *event)
{
    emit projectionChanged(updateProjection());

    Draggable::resizeEvent(event);
}

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

void Camera::move(int dx, int dy)
{
    const qreal scale = distanceFactor * qAbs(QVector3D::dotProduct(eye - origin, z)) / focalLength;

    move(scale * dx, -scale * dy, 0.);
}

void Camera::rotate(int dx, int dy)
{
    rotate(-angleFactor * dx, -angleFactor * dy, 0.);
}

void Camera::scale(int dx, int dy)
{
    Q_UNUSED(dy)
    move(0., 0., distanceFactor * dx);
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

    eye = origin + dq.rotatedVector(eye - origin);

    q = QQuaternion::fromAxisAndAngle(x, p);
    dq = QQuaternion::fromAxisAndAngle(x, -dp);

    y = q.rotatedVector(y);
    z = q.rotatedVector(z);

    eye = origin + dq.rotatedVector(eye - origin);

    q = QQuaternion::fromAxisAndAngle(z, b);
    dq = QQuaternion::fromAxisAndAngle(z, -db);

    x = q.rotatedVector(x);
    y = q.rotatedVector(y);

    eye = origin + dq.rotatedVector(eye - origin);

    /* update scene */
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
    const qreal aspectRatio = (qreal)width() / height();

    projection.setToIdentity();

    if (aspectRatio < RenderSettings::aspectRatio)
    {
        projection.rotate(-90, {0, 0, 1});
        projection.perspective(horizontalAngle, 1. / aspectRatio, .1, 1000.);
        projection.rotate(+90, {0, 0, 1});
    }
    else
    {
        projection.perspective(verticalAngle, aspectRatio, .1, 1000.);
    }

    return projection;
}

void Camera::updateAngles()
{
    horizontalAngle = (qreal)2.f * qRadiansToDegrees(qAtan(apertureWidth / 2 / focalLength));
    verticalAngle = (qreal)2.f * qRadiansToDegrees(qAtan(apertureWidth / RenderSettings::aspectRatio / 2 / focalLength));
}
