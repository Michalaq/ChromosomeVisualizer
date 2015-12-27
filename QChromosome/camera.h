#ifndef CAMERA_H
#define CAMERA_H

#include <QObject>

#include <QVector3D>
#include <QMatrix4x4>

class Camera : public QObject
{
    Q_OBJECT
public:
    explicit Camera(QObject *parent = 0);

    /* returns modelview matrix */
    QMatrix4x4 getModelView() const;

public slots:
    /* moves camera with respect to current coordinates system */
    void move(qreal dx, qreal dy, qreal dz);

    /* rotates camera with respect to current coordinates system */
    void rotate(qreal dh, qreal dp, qreal db);

private:
    /* eye position */
    QVector3D eye;

    /* direction vectors */
    QVector3D x, y, z;

    /* Euler angles */
    qreal h, p, b;

    QVector3D origin;

    QMatrix4x4 modelView;

    void updateModelView();
};

#endif // CAMERA_H
