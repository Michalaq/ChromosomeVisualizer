#ifndef CAMERA_H
#define CAMERA_H

#include "draggable.h"

#include <QVector3D>
#include <QMatrix4x4>

class Camera : public Draggable
{
    Q_OBJECT
public:
    explicit Camera(QWidget *parent = 0);

protected:
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);
    void resizeEvent(QResizeEvent *event);
    void wheelEvent(QWheelEvent *event);

    void connectNotify(const QMetaMethod &signal);

public slots:
    /* handles mouse move event */
    void move(int dx, int dy);

    /* handles mouse rotate event */
    void rotate(int dx, int dy);

    /* handles mouse scale event */
    void scale(int dx, int dy);

private:
    /* eye position */
    QVector3D eye;

    /* direction vectors */
    QVector3D x, y, z;

    /* Euler angles */
    qreal h, p, b;

    /* moves camera with respect to current coordinates system */
    void move(qreal dx, qreal dy, qreal dz);

    /* rotates camera with respect to current coordinates system */
    void rotate(qreal dh, qreal dp, qreal db);

    QVector3D origin;

    qreal verticalAngle;

    qreal focalLength;

    /* returns modelview matrix */
    QMatrix4x4 modelView() const;

    /* returns projection matrix */
    QMatrix4x4 projection() const;

    const qreal distanceFactor = 0.25;
    const qreal angleFactor = 0.05;
    const qreal wheelFactor = 2.00;

    Qt::Key modifier;

signals:
    void modelViewChanged(QMatrix4x4);
    void projectionChanged(QMatrix4x4);
};

#endif // CAMERA_H
