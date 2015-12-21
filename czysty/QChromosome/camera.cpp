#include "camera.h"

Camera::Camera(QObject *parent)
    : QObject(parent), eye(QVector3D(0, 50, 400)), x(QVector3D(1, 0, 0)), y(QVector3D(0, 1, 0)), z(QVector3D(0, 0, 1))
{
    updateModelView();
}

void Camera::updateModelView()
{
    modelView = QMatrix4x4(QQuaternion::fromAxes(x, y, z).toRotationMatrix()).inverted();
    modelView.translate(-eye);
}

void Camera::move(qreal dx, qreal dy, qreal dz)
{
    /* change to global coordinates */
    QVector3D delta = dx * x + dy * y + dz * z;

    /* update eye position */
    eye -= delta;

    /* update modelview matrix */
    updateModelView();
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

    /* update modelview matrix */
    updateModelView();
}

QMatrix4x4 Camera::getModelView() const
{
    return modelView;
}
