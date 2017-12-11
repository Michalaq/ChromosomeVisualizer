#include "camera.h"

const qreal Camera::distanceFactor = 0.025;
const qreal Camera::angleFactor = 0.05;
const qreal Camera::wheelFactor = 2.00;

bool Camera::lockX = false;
bool Camera::lockY = false;
bool Camera::lockZ = false;

QVector3D Camera::origin = {0, 0, 0};

Camera::Action Camera::currentAction;

bool Camera::automaticKeyframing = false;

Viewport* Camera::viewport = nullptr;

#include <QtMath>

Camera::Camera(QWidget *parent)
    : SplineInterpolator({"X", "Y", "Z", "H", "P", "B", "Focal length", "Aperture width"}, parent),
      eye(60, 30, 60),
      x(1, 0, 0),
      y(0, 1, 0),
      z(0, 0, 1),
      h(45), p(-20), b(0),
      focalLength(36),
      apertureWidth(36),
      rotationType(RT_World),
      nearClipping(.1),
      farClipping(1000.)
{
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

    QQuaternion q = QQuaternion::fromEulerAngles(p, h, b);

    x = q.rotatedVector(x);
    y = q.rotatedVector(y);
    z = q.rotatedVector(z);

    updateModelView();

    updateAngles();

    connect(this, &Camera::delta, [this](int dx, int dy) {
        switch (currentAction) {
        case CA_Move:
            move(dx, dy);
            break;
        case CA_Rotate:
            rotate(dx, dy);
            break;
        case CA_Scale:
            scale(dx, dy);
            break;
        };
    });
}

Camera::Camera(const Camera& camera)
    : SplineInterpolator({"X", "Y", "Z", "H", "P", "B", "Focal length", "Aperture width"}),
      eye(camera.eye),
      x(camera.x), y(camera.y), z(camera.z),
      h(camera.h), p(camera.p), b(camera.b),
      focalLength(camera.focalLength),
      apertureWidth(camera.apertureWidth),
      horizontalAngle(camera.horizontalAngle),
      verticalAngle(camera.verticalAngle),
      modelView(camera.modelView),
      projection(camera.projection),
      aspectRatio(camera.aspectRatio),
      rotationType(camera.rotationType),
      nearClipping(camera.nearClipping),
      farClipping(camera.farClipping)
{
    connect(this, &Camera::delta, [this](int dx, int dy) {
        switch (currentAction) {
        case CA_Move:
            move(dx, dy);
            break;
        case CA_Rotate:
            rotate(dx, dy);
            break;
        case CA_Scale:
            scale(dx, dy);
            break;
        };
    });
}

Camera::~Camera()
{

}

SplineKeyframe Camera::saveFrame() const
{
    SplineKeyframe f;

    f.insert("X", eye.x());
    f.insert("Y", eye.y());
    f.insert("Z", eye.z());
    f.insert("H", h);
    f.insert("P", p);
    f.insert("B", b);
    f.insert("Focal length", focalLength);
    f.insert("Aperture width", apertureWidth);

    return f;
}

void Camera::loadFrame(const SplineKeyframe &frame)
{
    QVector3D _eye;
    _eye.setX(frame.value("X", eye.x()));
    _eye.setY(frame.value("Y", eye.y()));
    _eye.setZ(frame.value("Z", eye.z()));
    setPosition(_eye);

    double _h = frame.value("H", h);
    double _p = frame.value("P", p);
    double _b = frame.value("B", b);
    setEulerAgnles(_h, _p, _b);

    focalLength = frame.value("Focal length", focalLength);
    apertureWidth = frame.value("Aperture width", apertureWidth);
    updateAngles();
}

void Camera::setViewport(Viewport *vp)
{
    viewport = vp;
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

void Camera::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);

    emit modelViewChanged(modelView);
    emit projectionChanged(projection);
}

#include <QPainter>
#include "viewport.h"

void Camera::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);

    if (viewport->getSFVisible())
    {
        int w = std::min(width(), int(qreal(height()) * aspectRatio + 0.5));
        int h = std::min(height(), int(qreal(width()) / aspectRatio + 0.5));

        QRect view(0, 0, w, h);
        view.moveCenter(rect().center());

        auto opacity = viewport->getSFOpacity();
        auto color = viewport->getSFColor();

        QPainter p(this);
        p.setOpacity(opacity);

        p.fillRect(QRect(rect().topLeft(), view.bottomLeft() - QPoint(1, 0)), color);
        p.fillRect(QRect(rect().topLeft(), view.topRight() - QPoint(0, 1)), color);

        p.fillRect(QRect(view.topRight() + QPoint(1, 0), rect().bottomRight()), color);
        p.fillRect(QRect(view.bottomLeft() + QPoint(0, 1), rect().bottomRight()), color);
    }

    auto position = viewport->getAxisPosition();

    if (position != Off_)
    {
        struct
        {
            QVector3D vector;
            Qt::GlobalColor color;
            QChar label;
        }
        axis[] =
        {
            {modelView.mapVector({1, 0, 0}), Qt::red,   'X'},
            {modelView.mapVector({0, 1, 0}), Qt::green, 'Y'},
            {modelView.mapVector({0, 0, 1}), Qt::blue,  'Z'}
        };

        if (axis[0].vector.z() > axis[1].vector.z()) std::swap(axis[0], axis[1]);
        if (axis[1].vector.z() > axis[2].vector.z()) std::swap(axis[1], axis[2]);
        if (axis[0].vector.z() > axis[1].vector.z()) std::swap(axis[0], axis[1]);

        QRectF r(0, 0, 20, 20);

        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);

        auto scale = viewport->getAxisScale();

        p.translate(position == TopLeft || position == BottomLeft ? 50 * scale : width() - 50 * scale,
                    position == TopLeft || position == TopRight ? 50 * scale : height() - 50 * scale);
        p.scale(scale, scale);

        for (auto a : axis)
        {
            p.setPen(QPen(a.color, 2, Qt::SolidLine, Qt::RoundCap));
            p.drawLine({0,0}, QPointF(a.vector.x(), -a.vector.y()) * 30);

            if (viewport->getAxisTextVisible())
            {
                r.moveCenter(QPointF(a.vector.x(), -a.vector.y()) * 40);
                p.drawText(r, Qt::AlignCenter, a.label);
            }
        }
    }
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
    return QVector3D(p, h, b);
}

void Camera::setPosition(const QVector3D &p)
{
    eye = p;

    emit modelViewChanged(updateModelView());
}

void Camera::move(int dx, int dy)
{
    const qreal scale = distanceFactor * qAbs(QVector3D::dotProduct(eye - origin, z)) / focalLength;

    move(scale * dx, -scale * dy, 0.);

    if (automaticKeyframing)
    {
        captureFrame();
        emit interpolationChanged();
    }
}

void Camera::rotate(int dx, int dy)
{
    rotate(-angleFactor * dx, -angleFactor * dy, 0.);

    if (automaticKeyframing)
    {
        captureFrame();
        emit interpolationChanged();
    }
}

void Camera::scale(int dx, int)
{
    move(0., 0., distanceFactor * dx);

    if (automaticKeyframing)
    {
        captureFrame();
        emit interpolationChanged();
    }
}

void Camera::setAspectRatio(qreal ar)
{
    aspectRatio = ar;
    updateAngles();
}

void Camera::lockCoordinates(bool x, bool y, bool z)
{
    lockX = x;
    lockY = y;
    lockZ = z;
}

void Camera::setCurrentAction(Action ca)
{
    currentAction = ca;
}

void Camera::move(qreal dx, qreal dy, qreal dz)
{
    /* change to global coordinates */
    QVector3D delta = x * dx + y * dy + z * dz;

    if (lockX) delta.setX(0);
    if (lockY) delta.setY(0);
    if (lockZ) delta.setZ(0);

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

#include <QJsonObject>
#include <QJsonArray>

void Camera::read(const QJsonObject &json)
{
    const QJsonObject coordinates = json["Coordinates"].toObject();
    eye.setX(coordinates["X"].toDouble());
    eye.setY(coordinates["Y"].toDouble());
    eye.setZ(coordinates["Z"].toDouble());
    h = coordinates["H"].toDouble();
    p = coordinates["P"].toDouble();
    b = coordinates["B"].toDouble();

    const QJsonObject objectProperties = json["Object properties"].toObject();
    focalLength = objectProperties["Focal length"].toDouble();
    apertureWidth = objectProperties["Aperture width"].toDouble();
    rotationType = objectProperties["Rotation type"].toInt();

    const QJsonObject depthOfField = json["Depth of field"].toObject();
    nearClipping = depthOfField["Near clipping"].toDouble();
    farClipping = depthOfField["Far clipping"].toDouble();

    QQuaternion q = QQuaternion::fromEulerAngles(p, h, b);

    x = q.rotatedVector({1,0,0});
    y = q.rotatedVector({0,1,0});
    z = q.rotatedVector({0,0,1});

    emit modelViewChanged(updateModelView());

    updateAngles();

    const QJsonArray keyframes = json["Key frames"].toArray();
    SplineInterpolator::read(keyframes);
}

void Camera::write(QJsonObject &json) const
{
    QJsonObject coordinates;
    coordinates["X"] = eye.x();
    coordinates["Y"] = eye.y();
    coordinates["Z"] = eye.z();
    coordinates["H"] = h;
    coordinates["P"] = p;
    coordinates["B"] = b;
    json["Coordinates"] = coordinates;

    QJsonObject objectProperties;
    objectProperties["Focal length"] = focalLength;
    objectProperties["Aperture width"] = apertureWidth;
    objectProperties["Rotation type"] = rotationType;
    json["Object properties"] = objectProperties;

    QJsonObject depthOfField;
    depthOfField["Near clipping"] = nearClipping;
    depthOfField["Far clipping"] = farClipping;
    json["Depth of field"] = depthOfField;

    QJsonArray keyframes;
    SplineInterpolator::write(keyframes);
    json["Key frames"] = keyframes;
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

    modelView = modelView.inverted();

    update();

    return modelView;
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

#include "moviemaker.h"

void Camera::writePOVCamera(std::ostream &stream, bool interpolate) const
{
    if (interpolate)
    {
        stream << "#declare MySplinePos = \n";
        writePOVSpline(stream, [](std::ostream &stream, const SplineKeyframe &frame) {
            stream << QVector3D(frame.value("X"), frame.value("Y"), frame.value("Z"));
        });

        stream << "#declare MySplineAng = \n";
        writePOVSpline(stream, [](std::ostream &stream, const SplineKeyframe &frame) {
            stream << QVector3D(frame.value("P"), frame.value("H"), frame.value("B"));
        });

        qreal d = aspectRatio / qSqrt(1. + aspectRatio * aspectRatio);

        stream << "#declare MySplineFov = \n";
        writePOVSpline(stream, [=](std::ostream &stream, const SplineKeyframe &frame) {
            stream << "< " << (qreal)2.f * qRadiansToDegrees(qAtan(frame.value("Aperture width") * d / 2 / frame.value("Focal length"))) << ", 0 >";
        });

        stream << "camera { perspective\n"
               << "right x * " << aspectRatio << "\n"
               << "look_at -z\n"
               << "angle MySplineFov(clock).x\n"
               << "rotate -MySplineAng(clock)\n"
               << "translate MySplinePos(clock)\n"
               << "}\n"
               << "\n";
    }
    else
        stream << "camera { perspective\n"
               << "right x * " << aspectRatio << "\n"
               << "look_at -z\n"
               << "angle " << horizontalAngle << "\n"
               << "rotate " << -QVector3D(p, h, b) << "\n"
               << "translate " << eye << "\n"
               << "}\n"
               << "\n";
}

void Camera::setAutomaticKeyframing(bool b)
{
    automaticKeyframing = b;
}
