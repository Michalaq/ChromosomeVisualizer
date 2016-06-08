#ifndef CAMERA_H
#define CAMERA_H

#include "draggable.h"

#include <QVector3D>
#include <QMatrix4x4>
#include <QStack>
#include <QMetaMethod>

class Camera : public Draggable
{
    Q_OBJECT
public:
    explicit Camera(QWidget *parent = 0);

protected:
    void resizeEvent(QResizeEvent *event);
    void wheelEvent(QWheelEvent *event);

    void connectNotify(const QMetaMethod &signal);

public:
    /* sets new origin */
    void setOrigin(const QVector3D& o);

    QVector3D position() const;
    void setPosition(const QVector3D& p);

    QVector3D lookAt() const;

    qreal angle() const;

public slots:
    /* handles mouse move event */
    void move(int dx, int dy);

    /* handles mouse rotate event */
    void rotate(int dx, int dy);

    /* handles mouse scale event */
    void scale(int dx, int dy);

    /* updates aspect ratio */
    void setAspectRatio(qreal ar);

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

    qreal focalLength;
    qreal apertureWidth;

    qreal horizontalAngle;
    qreal verticalAngle;

    QVector3D origin;

    QMatrix4x4 modelView;
    QMatrix4x4 projection;

    /* updates modelview matrix */
    QMatrix4x4& updateModelView();

    /* updates projection matrix */
    QMatrix4x4& updateProjection();

    /* updates field of view */
    void updateAngles();

    static const qreal distanceFactor;
    static const qreal angleFactor;
    static const qreal wheelFactor;

    qreal aspectRatio;

signals:
    void modelViewChanged(QMatrix4x4);
    void projectionChanged(QMatrix4x4);
};

#endif // CAMERA_H
