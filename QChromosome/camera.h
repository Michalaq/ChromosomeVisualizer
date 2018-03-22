#ifndef CAMERA_H
#define CAMERA_H

#include "interpolator.h"

#include <QVector3D>
#include <QMatrix4x4>

class Session;
class Viewport;
struct VizCameraInstance;

class Camera : public SplineInterpolator
{
    Q_OBJECT
public:
    explicit Camera(Session* s = nullptr, QWidget *parent = 0);
    Camera(const Camera& camera, Session* s);
    ~Camera();

    static void setViewport(Viewport* vp);

    /* sets new origin */
    static QVector3D getOrigin();
    static void setOrigin(const QVector3D& o);

    QVector3D getPosition() const;
    QVector3D getRotation() const;
    qreal getFocalLength() const;
    qreal getSensorSize() const;
    qreal getHorizontalAngle() const;
    qreal getVerticalAngle() const;
    qreal getNearClipping() const;
    qreal getFarClipping() const;

    void setPosition(const QVector3D& p);
    void setRotation(qreal h_, qreal p_, qreal b_);
    void setFocalLength(qreal fl);
    void setSensorSize(qreal ss);
    void setHorizontalAngle(qreal ha);
    void setVerticalAngle(qreal va);
    void setNearClipping(qreal nc);
    void setFarClipping(qreal fc);

    void setRotationType(int rt);
    void setLookAt(const QVector3D& target);

    enum RotationType
    {
        RT_World,
        RT_Camera
    };

    static void lockCoordinates(bool x, bool y, bool z);

    enum Action {
        CA_Move,
        CA_Rotate,
        CA_Scale
    };

    static void setCurrentAction(Action ca);

    void read(const QJsonObject& json);
    void write(QJsonObject& json) const;

    SplineKeyframe saveFrame() const;
    void loadFrame(const SplineKeyframe &frame);

    void writePOVCamera(std::ostream &stream, bool interpolate) const;

    static void setAutomaticKeyframing(bool b = true);

    void setBase(const QModelIndex& index);
    const QModelIndex& getBase() const;

    void setTarget(const QModelIndex& index);
    const QModelIndex& getTarget() const;

    void setUp(const QModelIndex& index);
    const QModelIndex& getUp() const;

public slots:
    /* handles mouse move event */
    void move(int dx, int dy);

    /* handles mouse rotate event */
    void rotate(int dx, int dy);

    /* handles mouse scale event */
    void scale(int dx, int dy);

    /* updates aspect ratio */
    void setAspectRatio(qreal ar);

protected:
    void resizeEvent(QResizeEvent *event);
    void wheelEvent(QWheelEvent *event);

    void connectNotify(const QMetaMethod &signal);
    void showEvent(QShowEvent *event);

    void paintEvent(QPaintEvent *event);

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
    qreal sensorSize;

    qreal horizontalAngle;
    qreal verticalAngle;

    static QVector3D origin;

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

    int rotationType;

    qreal nearClipping;
    qreal farClipping;

    static bool lockX, lockY, lockZ;

    static Action currentAction;

    static bool automaticKeyframing;

    static Viewport* viewport;

    QPersistentModelIndex base, target, up;

    Session* session;
    int id;

signals:
    void modelViewChanged(QMatrix4x4, QObject* = Q_NULLPTR);
    void projectionChanged(QMatrix4x4, QObject* = Q_NULLPTR);

friend class CameraItem;
};

#endif // CAMERA_H
