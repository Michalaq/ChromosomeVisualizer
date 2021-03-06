#include "camera.h"

const qreal Camera::distanceFactor = 0.06;
const qreal Camera::angleFactor = 0.50;
const qreal Camera::scaleFactor = 0.75;
const qreal Camera::wheelFactor = 0.15;
const qreal Camera::zoomFactor = 0.01;

bool Camera::lockX = false;
bool Camera::lockY = false;
bool Camera::lockZ = false;

Camera::Action Camera::currentAction;

#include "treeitem.h"
#include "session.h"

Camera::Camera(Session *s, QWidget *parent)
    : SplineInterpolator({"X", "Y", "Z", "H", "P", "B", "Focal length", "Sensor size", "Zoom"}, parent),
      action(new QAction),
      eye(60, 30, 60),
      focalLength(36),
      sensorSize(36),
      nearClipping(.3),
      farClipping(1000.),
      session(s),
      id(session->cameraBuffer.append({})),
      mode(CM_Mono),
      eyeSeparation(6.5),
      zoom(1.0)
{
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

    setLookAt(QVector3D(0, 0, 0));

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

    connect(session->renderSettings, &TabWidget::filmRatioChanged, this, &Camera::setAspectRatio);

    action->setIcon(QPixmap(":/dialogs/film camera"));
    action->setCheckable(true);
    session->cameras->addAction(action);
    session->camerasGroup->addAction(action);

    connect(action, &QAction::triggered, [this] {
        session->simulation->getModel()->changeCamera(this);
    });
}

Camera::Camera(const Camera& camera)
    : SplineInterpolator({"X", "Y", "Z", "H", "P", "B", "Focal length", "Sensor size", "Zoom"}),
      action(new QAction),
      eye(camera.eye),
      phb(camera.phb),
      focalLength(camera.focalLength),
      sensorSize(camera.sensorSize),
      horizontalAngle(camera.horizontalAngle),
      verticalAngle(camera.verticalAngle),
      modelView(camera.modelView),
      projection(camera.projection),
      aspectRatio(camera.aspectRatio),
      nearClipping(camera.nearClipping),
      farClipping(camera.farClipping),
      session(camera.session),
      id(session->cameraBuffer.append(session->cameraBuffer[camera.id])),
      mode(camera.mode),
      eyeSeparation(camera.eyeSeparation),
      zoom(camera.zoom)
{
    resize(camera.size());

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

    connect(session->renderSettings, &TabWidget::filmRatioChanged, this, &Camera::setAspectRatio);

    action->setIcon(QPixmap(":/dialogs/film camera"));
    action->setCheckable(true);
    session->cameras->addAction(action);
    session->camerasGroup->addAction(action);

    connect(action, &QAction::triggered, [this] {
        session->simulation->getModel()->changeCamera(this);
    });
}

Camera::~Camera()
{
    delete action;
}

SplineKeyframe Camera::saveFrame() const
{
    SplineKeyframe f;

    auto angles = phb.toEulerAngles();

    f.insert("X", eye[0]);
    f.insert("Y", eye[1]);
    f.insert("Z", eye[2]);
    f.insert("P", angles[0]);
    f.insert("H", angles[1]);
    f.insert("B", angles[2]);
    f.insert("Focal length", focalLength);
    f.insert("Sensor size", sensorSize);
    f.insert("Zoom", zoom);

    return f;
}

void Camera::loadFrame(const SplineKeyframe &frame)
{
    auto angles = phb.toEulerAngles();

    auto x = frame.value("X", eye[0]);
    auto y = frame.value("Y", eye[1]);
    auto z = frame.value("Z", eye[2]);
    setPosition(QVector3D(x, y, z));

    auto p = frame.value("P", angles[0]);
    auto h = frame.value("H", angles[1]);
    auto b = frame.value("B", angles[2]);
    setRotation(QQuaternion::fromEulerAngles(p, h, b));

    focalLength = frame.value("Focal length", focalLength);
    sensorSize = frame.value("Sensor size", sensorSize);
    zoom = frame.value("Zoom", zoom);
    updateAngles();
}

#include <QResizeEvent>

void Camera::resizeEvent(QResizeEvent *event)
{
    emit projectionChanged(updateProjection());

    Draggable::resizeEvent(event);
}

#include <QWheelEvent>

void Camera::wheelEvent(QWheelEvent *event)
{
    scale(wheelFactor * event->angleDelta().y(), wheelFactor * event->angleDelta().y());

    event->accept();
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

void Camera::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);

    if (session->viewport->getSFVisible())
    {
        int w = std::min(width(), int(qreal(height()) * aspectRatio + 0.5));
        int h = std::min(height(), int(qreal(width()) / aspectRatio + 0.5));

        QRect view(0, 0, w, h);
        view.moveCenter(rect().center());

        auto opacity = session->viewport->getSFOpacity();
        auto color = session->viewport->getSFColor();

        QPainter p(this);
        p.setOpacity(opacity);

        p.fillRect(QRect(rect().topLeft(), view.bottomLeft() - QPoint(1, 0)), color);
        p.fillRect(QRect(rect().topLeft(), view.topRight() - QPoint(0, 1)), color);

        p.fillRect(QRect(view.topRight() + QPoint(1, 0), rect().bottomRight()), color);
        p.fillRect(QRect(view.bottomLeft() + QPoint(0, 1), rect().bottomRight()), color);
    }

    auto position = session->viewport->getAxisPosition();

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

        auto scale = session->viewport->getAxisScale();

        p.translate(position == TopLeft || position == BottomLeft ? 50 * scale : width() - 50 * scale,
                    position == TopLeft || position == TopRight ? 50 * scale : height() - 50 * scale);
        p.scale(scale, scale);

        for (auto a : axis)
        {
            p.setPen(QPen(a.color, 2, Qt::SolidLine, Qt::RoundCap));
            p.drawLine({0,0}, QPointF(a.vector.x(), -a.vector.y()) * 30);

            if (session->viewport->getAxisTextVisible())
            {
                r.moveCenter(QPointF(a.vector.x(), -a.vector.y()) * 40);
                p.drawText(r, Qt::AlignCenter, a.label);
            }
        }
    }

    static const char* pnames[] = {"Perspective", "Parallel"};

    const char* pname = pnames[session->cameraBuffer[id].ptype];

    QRect r(2, 2, fontMetrics().width(pname) + 6, fontMetrics().height() + 6);

    QPainter p(this);
    p.save();
    p.setPen(Qt::NoPen);
    p.setBrush(Qt::black);
    p.setOpacity(0.5);

    p.drawRoundedRect(r, 3, 3);

    p.restore();
    p.setPen(Qt::white);
    p.drawText(r, Qt::AlignCenter, pname);
}

QVector3D Camera::getPosition() const
{
    return eye;
}

QQuaternion Camera::getRotation() const
{
    return phb;
}

qreal Camera::getFocalLength() const
{
    return focalLength;
}

qreal Camera::getSensorSize() const
{
    return sensorSize;
}

qreal Camera::getHorizontalAngle() const
{
    return horizontalAngle;
}

qreal Camera::getVerticalAngle() const
{
    return verticalAngle;
}

qreal Camera::getNearClipping() const
{
    return nearClipping;
}

qreal Camera::getFarClipping() const
{
    return farClipping;
}

void Camera::setPosition(const QVector3D &p)
{
    eye = p;

    emit modelViewChanged(updateModelView());
}

void Camera::move(int dx, int dy)
{
    QVector3D x, y, z;
    phb.getAxes(&x, &y, &z);

    const qreal scale = distanceFactor * qMax(qAbs(QVector3D::dotProduct(eye - session->origin, z)), 1.f) / focalLength;

    move(scale * dx, -scale * dy, 0.);

    if (session->autokeying)
    {
        captureFrame();
        emit interpolationChanged();
    }
}

void Camera::rotate(int dx, int dy)
{
    rotate(angleFactor * dy, angleFactor * dx, 0);

    if (session->autokeying)
    {
        captureFrame();
        emit interpolationChanged();
    }
}

#include <QtMath>

void Camera::scale(int dx, int)
{
    switch (session->cameraBuffer[id].ptype)
    {
    case CP_Perspective:
        move(0., 0., scaleFactor * dx);
        break;
    case CP_Parallel:
        zoom *= qExp(zoomFactor * dx);
        emit projectionChanged(updateProjection());
        break;
    }

    if (session->autokeying)
    {
        captureFrame();
        emit interpolationChanged();
    }
}

void Camera::setAspectRatio(qreal ar)
{
    aspectRatio = ar;
    updateAngles();
    update();
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
    QVector3D x, y, z;
    phb.getAxes(&x, &y, &z);

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

void Camera::setRotation(const QQuaternion &q)
{
    phb = q;
    emit modelViewChanged(updateModelView());
}

void Camera::setFocalLength(qreal fl)
{
    focalLength = fl;
    updateAngles();
}

void Camera::setSensorSize(qreal ss)
{
    sensorSize = ss;
    updateAngles();
}

void Camera::setHorizontalAngle(qreal ha)
{
    qreal w = sensorSize;

    setFocalLength(w / 2 / qTan(qDegreesToRadians(ha / 2)));
}

void Camera::setVerticalAngle(qreal va)
{
    qreal h = sensorSize / aspectRatio;

    setFocalLength(h / 2 / qTan(qDegreesToRadians(va / 2)));
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
    auto p = qRadiansToDegrees(qAtan2(f.y(), QVector2D(f.x(), -f.z()).length()));
    auto h = qRadiansToDegrees(qAtan2(-f.x(), -f.z()));
    auto b = 0;
    setRotation(QQuaternion::fromEulerAngles(p, h, b));
}

#include <QJsonObject>
#include <QJsonArray>

void Camera::read(const QJsonObject &json)
{
    const QJsonObject coordinates = json["Coordinates"].toObject();
    auto x = coordinates["X"].toDouble();
    auto y = coordinates["Y"].toDouble();
    auto z = coordinates["Z"].toDouble();
    eye = QVector3D(x, y, z);

    auto p = coordinates["P"].toDouble();
    auto h = coordinates["H"].toDouble();
    auto b = coordinates["B"].toDouble();
    phb = QQuaternion::fromEulerAngles(p, h, b);

    emit modelViewChanged(updateModelView());

    const QJsonObject objectProperties = json["Object properties"].toObject();
    session->cameraBuffer[id].ptype = static_cast<Projection>(objectProperties["Projection"].toInt());
    focalLength = objectProperties["Focal length"].toDouble();
    sensorSize = objectProperties["Sensor size"].toDouble();
    zoom = objectProperties["Zoom"].toDouble();

    const QJsonObject details = json["Details"].toObject();
    nearClipping = details["Near clipping"].toDouble();
    farClipping = details["Far clipping"].toDouble();

    updateAngles();

    const QJsonObject stereoscopic = json["Stereoscopic"].toObject();
    mode = static_cast<Mode>(stereoscopic["Mode"].toInt());
    eyeSeparation = stereoscopic["Eye separation"].toDouble();

    const QJsonArray keyframes = json["Key frames"].toArray();
    SplineInterpolator::read(keyframes);
}

void Camera::write(QJsonObject &json) const
{
    auto angles = phb.toEulerAngles();

    QJsonObject coordinates;
    coordinates["X"] = eye[0];
    coordinates["Y"] = eye[1];
    coordinates["Z"] = eye[2];
    coordinates["P"] = angles[0];
    coordinates["H"] = angles[1];
    coordinates["B"] = angles[2];
    json["Coordinates"] = coordinates;

    QJsonObject objectProperties;
    objectProperties["Projection"] = session->cameraBuffer[id].ptype;
    objectProperties["Focal length"] = focalLength;
    objectProperties["Sensor size"] = sensorSize;
    objectProperties["Zoom"] = zoom;
    json["Object properties"] = objectProperties;

    QJsonObject details;
    details["Near clipping"] = nearClipping;
    details["Far clipping"] = farClipping;
    json["Details"] = details;

    QJsonObject stereoscopic;
    stereoscopic["Mode"] = mode;
    stereoscopic["Eye separation"] = eyeSeparation;
    json["Stereoscopic"] = stereoscopic;

    QJsonArray keyframes;
    SplineInterpolator::write(keyframes);
    json["Key frames"] = keyframes;
}

void Camera::rotate(qreal dp, qreal dh, qreal db)
{
    /* get direction vectors */
    QVector3D x, y, z;
    phb.getAxes(&x, &y, &z);

    QQuaternion dq;

    /* rotate local coordinates */
    dq = QQuaternion::fromAxisAndAngle(y, -dh);
    phb = dq * phb;

    x = dq.rotatedVector(x);
    z = dq.rotatedVector(z);

    eye = session->origin + dq.rotatedVector(eye - session->origin);

    dq = QQuaternion::fromAxisAndAngle(x, -dp);
    phb = dq * phb;

    z = dq.rotatedVector(z);

    eye = session->origin + dq.rotatedVector(eye - session->origin);

    dq = QQuaternion::fromAxisAndAngle(z, -db);
    phb = dq * phb;

    eye = session->origin + dq.rotatedVector(eye - session->origin);

    emit modelViewChanged(updateModelView());
}

QMatrix4x4& Camera::updateModelView()
{
    modelView.setToIdentity();

    modelView.translate(eye);
    modelView.rotate(phb);

    update();

    return session->cameraBuffer[id].modelView = modelView = modelView.inverted();
}

QMatrix4x4& Camera::updateProjection()
{
    const qreal aspectRatio_ = (qreal)width() / height();

    projection.setToIdentity();

    switch (session->cameraBuffer[id].ptype)
    {
    case CP_Perspective:
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

        break;

    case CP_Parallel:
        if (aspectRatio_ < aspectRatio)
        {
            auto w = 100 / zoom;
            auto h = 100 / aspectRatio_ / zoom;

            projection.ortho(-w/2, +w/2, -h/2, +h/2, nearClipping, farClipping);
        }
        else
        {
            auto w = 100 * aspectRatio_ / aspectRatio / zoom;
            auto h = 100 / aspectRatio / zoom;

            projection.ortho(-w/2, +w/2, -h/2, +h/2, nearClipping, farClipping);
        }

        break;
    }

    return session->cameraBuffer[id].projection = projection;
}

void Camera::updateAngles()
{
    qreal w = sensorSize;
    qreal h = sensorSize / aspectRatio;

    horizontalAngle = (qreal)2.f * qRadiansToDegrees(qAtan(w / 2 / focalLength));
    verticalAngle = (qreal)2.f * qRadiansToDegrees(qAtan(h / 2 / focalLength));

    emit projectionChanged(updateProjection());
}

#include "moviemaker.h"

void Camera::writePOVCamera(QTextStream &stream, bool interpolate) const
{
    interpolate = interpolate && count() > 1;

    switch (mode)
    {
    case CM_Mono:
        if (interpolate)
        {
            stream << "#declare MySplinePos = \n";
            writePOVSpline(stream, [](QTextStream &stream, const SplineKeyframe &frame) {
                stream << QVector3D(frame.value("X"), frame.value("Y"), frame.value("Z"));
            });

            stream << "#declare MySplineAng = \n";
            writePOVSpline(stream, [](QTextStream &stream, const SplineKeyframe &frame) {
                stream << QVector3D(frame.value("P"), frame.value("H"), frame.value("B"));
            });

            stream << "#declare MySplineFov = \n";
            writePOVSpline(stream, [](QTextStream &stream, const SplineKeyframe &frame) {
                stream << "< " << (qreal)2.f * qRadiansToDegrees(qAtan(frame.value("Sensor size") / 2 / frame.value("Focal length"))) << ", 0 >";
            });

            switch (session->cameraBuffer[id].ptype)
            {
            case CP_Perspective:
                stream << "camera { perspective\n"
                       << "right x * " << aspectRatio << "\n"
                       << "look_at -z\n"
                       << "angle MySplineFov(clock).x\n"
                       << "rotate -<0, 0, MySplineAng(clock).z>\n"
                       << "rotate -<MySplineAng(clock).x, MySplineAng(clock).y, 0>\n"
                       << "translate MySplinePos(clock)\n"
                       << "}\n"
                       << "\n";
                break;

            case CP_Parallel:
                stream << "camera { orthographic\n"
                       << "right x * " << 100 / zoom << "\n"
                       << "up y * " << 100 / aspectRatio / zoom << "\n"
                       << "look_at -z\n"
                       << "rotate -<0, 0, MySplineAng(clock).z>\n"
                       << "rotate -<MySplineAng(clock).x, MySplineAng(clock).y, 0>\n"
                       << "translate MySplinePos(clock)\n"
                       << "}\n"
                       << "\n";
                break;
            }

            stream << "light_source {\n"
                   << QVector3D() << "," << QColor(Qt::white) << "\n"
                   << "parallel\n"
                   << "point_at " << -session->viewportUniformBuffer[0].uvDefaultLight << "\n"
                   << "rotate -<0, 0, MySplineAng(clock).z>\n"
                   << "rotate -<MySplineAng(clock).x, MySplineAng(clock).y, 0>\n"
                   << "}\n";
        }
        else
        {
            auto angles = phb.toEulerAngles();

            switch (session->cameraBuffer[id].ptype)
            {
            case CP_Perspective:
                stream << "camera { perspective\n"
                       << "right x * " << aspectRatio << "\n"
                       << "look_at -z\n"
                       << "angle " << horizontalAngle << "\n"
                       << "rotate -" << QVector3D(0, 0, angles.z()) << "\n"
                       << "rotate -" << QVector3D(angles.x(), angles.y(), 0) << "\n"
                       << "translate " << eye << "\n"
                       << "}\n"
                       << "\n";
                break;

            case CP_Parallel:
                stream << "camera { orthographic\n"
                       << "right x * " << 100 / zoom << "\n"
                       << "up y * " << 100 / aspectRatio / zoom << "\n"
                       << "look_at -z\n"
                       << "rotate -" << QVector3D(0, 0, angles.z()) << "\n"
                       << "rotate -" << QVector3D(angles.x(), angles.y(), 0) << "\n"
                       << "translate " << eye << "\n"
                       << "}\n"
                       << "\n";
                break;
            }

            stream << "light_source {\n"
                   << QVector3D() << "," << QColor(Qt::white) << "\n"
                   << "parallel\n"
                   << "point_at " << -session->viewportUniformBuffer[0].uvDefaultLight << "\n"
                   << "rotate -" << QVector3D(0, 0, angles.z()) << "\n"
                   << "rotate -" << QVector3D(angles.x(), angles.y(), 0) << "\n"
                   << "}\n";
        }
        break;

    case CM_Symmetrical:
        stream << "#declare odsIPD = " << eyeSeparation / 100 << ";\n";

        if (interpolate)
        {
            stream << "#declare MySplinePos = \n";
            writePOVSpline(stream, [](QTextStream &stream, const SplineKeyframe &frame) {
                stream << QVector3D(frame.value("X"), frame.value("Y"), frame.value("Z"));
            });

            stream << "#declare MySplineAng = \n";
            writePOVSpline(stream, [](QTextStream &stream, const SplineKeyframe &frame) {
                stream << QVector3D(frame.value("P"), frame.value("H"), frame.value("B"));
            });

            stream << "#declare odsLocation = MySplinePos(clock);\n"
                   << "#declare odsAngles = MySplineAng(clock);\n";
        }
        else
            stream << "#declare odsLocation = " << eye << ";\n"
                   << "#declare odsAngles = " << phb.toEulerAngles() << ";\n";

        stream << "#declare odsX = <1, 0, 0>;\n"
               << "#declare odsY = <0, 1, 0>;\n"
               << "#declare odsZ = <0, 0, 1>;\n"
               << "#declare odsX = vaxis_rotate(odsX, odsY, odsAngles.y);\n"
               << "#declare odsZ = vaxis_rotate(odsZ, odsY, odsAngles.y);\n"
               << "#declare odsY = vaxis_rotate(odsY, odsX, -odsAngles.x);\n"
               << "#declare odsZ = vaxis_rotate(odsZ, odsX, -odsAngles.x);\n"
               << "#declare odsX = vaxis_rotate(odsX, odsZ, odsAngles.z);\n"
               << "#declare odsY = vaxis_rotate(odsY, odsZ, odsAngles.z);\n";

        stream << "#declare odsLocationX = odsLocation.x;\n"
               << "#declare odsLocationY = odsLocation.y;\n"
               << "#declare odsLocationZ = odsLocation.z;\n"
               << "#declare odsXX = -odsX.x;\n"
               << "#declare odsXY = odsX.y;\n"
               << "#declare odsXZ = odsX.z;\n"
               << "#declare odsYX = -odsY.x;\n"
               << "#declare odsYY = odsY.y;\n"
               << "#declare odsYZ = odsY.z;\n"
               << "#declare odsZX = -odsZ.x;\n"
               << "#declare odsZY = odsZ.y;\n"
               << "#declare odsZZ = odsZ.z;\n";

        stream << "camera {\n"
               << "user_defined\n"
               << "location {\n"
               << "function { odsLocationX + cos(x * 2 * pi) * odsIPD/2 * select(y, 1, -1) * odsXX - sin(x * 2 * pi) * odsIPD/2 * select(y, 1, -1) * odsZX }\n"
               << "function { odsLocationY + cos(x * 2 * pi) * odsIPD/2 * select(y, 1, -1) * odsXY - sin(x * 2 * pi) * odsIPD/2 * select(y, 1, -1) * odsZY }\n"
               << "function { odsLocationZ + cos(x * 2 * pi) * odsIPD/2 * select(y, 1, -1) * odsXZ - sin(x * 2 * pi) * odsIPD/2 * select(y, 1, -1) * odsZZ }\n"
               << "}\n"
               << "direction {\n"
               << "function { sin(x * 2 * pi) * cos(pi / 2 + select(y, 2*y, 2*y-1) * pi) * odsXX + sin(pi / 2 + select(y, 2*y, 2*y-1) * pi) * odsYX - cos(x * 2 * pi) * cos(pi / 2 + select(y, 2*y, 2*y-1) * pi) * odsZX }\n"
               << "function { sin(x * 2 * pi) * cos(pi / 2 + select(y, 2*y, 2*y-1) * pi) * odsXY + sin(pi / 2 + select(y, 2*y, 2*y-1) * pi) * odsYY - cos(x * 2 * pi) * cos(pi / 2 + select(y, 2*y, 2*y-1) * pi) * odsZY }\n"
               << "function { sin(x * 2 * pi) * cos(pi / 2 + select(y, 2*y, 2*y-1) * pi) * odsXZ + sin(pi / 2 + select(y, 2*y, 2*y-1) * pi) * odsYZ - cos(x * 2 * pi) * cos(pi / 2 + select(y, 2*y, 2*y-1) * pi) * odsZZ }\n"
               << "}\n"
               << "}\n";

        stream << "light_source {\n"
               << QVector3D() << "," << QColor(Qt::white) << "\n"
               << "parallel\n"
               << "point_at " << -session->viewportUniformBuffer[0].uvDefaultLight << "\n"
               << "rotate -<0, 0, odsAngles.z>\n"
               << "rotate -<odsAngles.x, odsAngles.y, 0>\n"
               << "}\n";
        break;

    case CM_Left:
        stream << "#declare odsIPD = " << eyeSeparation / 100 << ";\n";

        if (interpolate)
        {
            stream << "#declare MySplinePos = \n";
            writePOVSpline(stream, [](QTextStream &stream, const SplineKeyframe &frame) {
                stream << QVector3D(frame.value("X"), frame.value("Y"), frame.value("Z"));
            });

            stream << "#declare MySplineAng = \n";
            writePOVSpline(stream, [](QTextStream &stream, const SplineKeyframe &frame) {
                stream << QVector3D(frame.value("P"), frame.value("H"), frame.value("B"));
            });

            stream << "#declare odsLocation = MySplinePos(clock);\n"
                   << "#declare odsAngles = MySplineAng(clock);\n";
        }
        else
            stream << "#declare odsLocation = " << eye << ";\n"
                   << "#declare odsAngles = " << phb.toEulerAngles() << ";\n";

        stream << "#declare odsX = <1, 0, 0>;\n"
               << "#declare odsY = <0, 1, 0>;\n"
               << "#declare odsZ = <0, 0, 1>;\n"
               << "#declare odsX = vaxis_rotate(odsX, odsY, odsAngles.y);\n"
               << "#declare odsZ = vaxis_rotate(odsZ, odsY, odsAngles.y);\n"
               << "#declare odsY = vaxis_rotate(odsY, odsX, -odsAngles.x);\n"
               << "#declare odsZ = vaxis_rotate(odsZ, odsX, -odsAngles.x);\n"
               << "#declare odsX = vaxis_rotate(odsX, odsZ, odsAngles.z);\n"
               << "#declare odsY = vaxis_rotate(odsY, odsZ, odsAngles.z);\n";

        stream << "#declare odsLocationX = odsLocation.x;\n"
               << "#declare odsLocationY = odsLocation.y;\n"
               << "#declare odsLocationZ = odsLocation.z;\n"
               << "#declare odsXX = -odsX.x;\n"
               << "#declare odsXY = odsX.y;\n"
               << "#declare odsXZ = odsX.z;\n"
               << "#declare odsYX = -odsY.x;\n"
               << "#declare odsYY = odsY.y;\n"
               << "#declare odsYZ = odsY.z;\n"
               << "#declare odsZX = -odsZ.x;\n"
               << "#declare odsZY = odsZ.y;\n"
               << "#declare odsZZ = odsZ.z;\n";

        stream << "camera {\n"
               << "user_defined\n"
               << "location {\n"
               << "function { odsLocationX - cos(x * 2 * pi) * odsIPD/2 * odsXX + sin(x * 2 * pi) * odsIPD/2 * odsZX }\n"
               << "function { odsLocationY - cos(x * 2 * pi) * odsIPD/2 * odsXY + sin(x * 2 * pi) * odsIPD/2 * odsZY }\n"
               << "function { odsLocationZ - cos(x * 2 * pi) * odsIPD/2 * odsXZ + sin(x * 2 * pi) * odsIPD/2 * odsZZ }\n"
               << "}\n"
               << "direction {\n"
               << "function { sin(x * 2 * pi) * cos(y * pi) * odsXX + sin(y * pi) * odsYX - cos(x * 2 * pi) * cos(y * pi) * odsZX }\n"
               << "function { sin(x * 2 * pi) * cos(y * pi) * odsXY + sin(y * pi) * odsYY - cos(x * 2 * pi) * cos(y * pi) * odsZY }\n"
               << "function { sin(x * 2 * pi) * cos(y * pi) * odsXZ + sin(y * pi) * odsYZ - cos(x * 2 * pi) * cos(y * pi) * odsZZ }\n"
               << "}\n"
               << "}\n";

        stream << "light_source {\n"
               << QVector3D() << "," << QColor(Qt::white) << "\n"
               << "parallel\n"
               << "point_at " << -session->viewportUniformBuffer[0].uvDefaultLight << "\n"
               << "rotate -<0, 0, odsAngles.z>\n"
               << "rotate -<odsAngles.x, odsAngles.y, 0>\n"
               << "}\n";
        break;

    case CM_Right:
        stream << "#declare odsIPD = " << eyeSeparation / 100 << ";\n";

        if (interpolate)
        {
            stream << "#declare MySplinePos = \n";
            writePOVSpline(stream, [](QTextStream &stream, const SplineKeyframe &frame) {
                stream << QVector3D(frame.value("X"), frame.value("Y"), frame.value("Z"));
            });

            stream << "#declare MySplineAng = \n";
            writePOVSpline(stream, [](QTextStream &stream, const SplineKeyframe &frame) {
                stream << QVector3D(frame.value("P"), frame.value("H"), frame.value("B"));
            });

            stream << "#declare odsLocation = MySplinePos(clock);\n"
                   << "#declare odsAngles = MySplineAng(clock);\n";
        }
        else
            stream << "#declare odsLocation = " << eye << ";\n"
                   << "#declare odsAngles = " << phb.toEulerAngles() << ";\n";

        stream << "#declare odsX = <1, 0, 0>;\n"
               << "#declare odsY = <0, 1, 0>;\n"
               << "#declare odsZ = <0, 0, 1>;\n"
               << "#declare odsX = vaxis_rotate(odsX, odsY, odsAngles.y);\n"
               << "#declare odsZ = vaxis_rotate(odsZ, odsY, odsAngles.y);\n"
               << "#declare odsY = vaxis_rotate(odsY, odsX, -odsAngles.x);\n"
               << "#declare odsZ = vaxis_rotate(odsZ, odsX, -odsAngles.x);\n"
               << "#declare odsX = vaxis_rotate(odsX, odsZ, odsAngles.z);\n"
               << "#declare odsY = vaxis_rotate(odsY, odsZ, odsAngles.z);\n";

        stream << "#declare odsLocationX = odsLocation.x;\n"
               << "#declare odsLocationY = odsLocation.y;\n"
               << "#declare odsLocationZ = odsLocation.z;\n"
               << "#declare odsXX = -odsX.x;\n"
               << "#declare odsXY = odsX.y;\n"
               << "#declare odsXZ = odsX.z;\n"
               << "#declare odsYX = -odsY.x;\n"
               << "#declare odsYY = odsY.y;\n"
               << "#declare odsYZ = odsY.z;\n"
               << "#declare odsZX = -odsZ.x;\n"
               << "#declare odsZY = odsZ.y;\n"
               << "#declare odsZZ = odsZ.z;\n";

        stream << "camera {\n"
               << "user_defined\n"
               << "location {\n"
               << "function { odsLocationX + cos(x * 2 * pi) * odsIPD/2 * odsXX - sin(x * 2 * pi) * odsIPD/2 * odsZX }\n"
               << "function { odsLocationY + cos(x * 2 * pi) * odsIPD/2 * odsXY - sin(x * 2 * pi) * odsIPD/2 * odsZY }\n"
               << "function { odsLocationZ + cos(x * 2 * pi) * odsIPD/2 * odsXZ - sin(x * 2 * pi) * odsIPD/2 * odsZZ }\n"
               << "}\n"
               << "direction {\n"
               << "function { sin(x * 2 * pi) * cos(y * pi) * odsXX + sin(y * pi) * odsYX - cos(x * 2 * pi) * cos(y * pi) * odsZX }\n"
               << "function { sin(x * 2 * pi) * cos(y * pi) * odsXY + sin(y * pi) * odsYY - cos(x * 2 * pi) * cos(y * pi) * odsZY }\n"
               << "function { sin(x * 2 * pi) * cos(y * pi) * odsXZ + sin(y * pi) * odsYZ - cos(x * 2 * pi) * cos(y * pi) * odsZZ }\n"
               << "}\n"
               << "}\n";

        stream << "light_source {\n"
               << QVector3D() << "," << QColor(Qt::white) << "\n"
               << "parallel\n"
               << "point_at " << -session->viewportUniformBuffer[0].uvDefaultLight << "\n"
               << "rotate -<0, 0, odsAngles.z>\n"
               << "rotate -<odsAngles.x, odsAngles.y, 0>\n"
               << "}\n";
        break;
    }
}

void Camera::setBase(const QModelIndex& index)
{
    base = index;
}

const QModelIndex& Camera::getBase() const
{
    return base;
}

void Camera::setTarget(const QModelIndex& index)
{
    target = index;
}

const QModelIndex& Camera::getTarget() const
{
    return target;
}

void Camera::setUp(const QModelIndex& index)
{
    up = index;
}

const QModelIndex& Camera::getUp() const
{
    return up;
}

void Camera::callibrate(const QModelIndex &index, bool selected, qreal scale)
{
    const auto model = session->simulation->getModel();

    if ((selected && !session->treeView->selectionModel()->hasSelection()) || model->rowCount(index) == 0)
        return;

    setRotation(QQuaternion::fromEulerAngles(-35.2644, -135, 0));

    qreal tha = qTan(qDegreesToRadians(horizontalAngle / 2)) * scale;
    qreal tva = qTan(qDegreesToRadians(verticalAngle / 2)) * scale;

    qreal sha = tha / qSqrt(1 + tha * tha);
    qreal sva = tva / qSqrt(1 + tva * tva);

    qreal dxr = -qInf(), dxl = -qInf(), dyr = -qInf(), dyl = -qInf(), tmp;

    std::function<void(const QModelIndex&)> visit = [&](const QModelIndex& root) {
        for (int r = 0; r < model->rowCount(root); r++)
            visit(model->index(r, 0, root));

        auto item = dynamic_cast<AtomItem*>(reinterpret_cast<TreeItem*>(root.internalPointer()));

        if (!item)
            return;

        const auto& atom = session->atomBuffer[item->getId()];

        if (selected && !atom.flags.testFlag(Selected))
            return;

        auto p = modelView.map(QVector4D(atom.position, 1)).toVector3DAffine();

        tmp = p.x() / (+tha) + p.z() + atom.size / sha;
        if (dxr < tmp)
            dxr = tmp;

        tmp = p.x() / (-tha) + p.z() + atom.size / sha;
        if (dxl < tmp)
            dxl = tmp;

        tmp = p.y() / (+tva) + p.z() + atom.size / sva;
        if (dyr < tmp)
            dyr = tmp;

        tmp = p.y() / (-tva) + p.z() + atom.size / sva;
        if (dyl < tmp)
            dyl = tmp;
    };

    visit(index);

    QVector3D x, y, z;
    phb.getAxes(&x, &y, &z);

    setPosition(eye + (dxr - dxl) / 2 * tha * x + (dyr - dyl) / 2 * tva * y + qMax((dxr + dxl) / 2, (dyr + dyl) / 2) * z);
}

Camera::Mode Camera::getMode() const
{
    return mode;
}

void Camera::setMode(Mode cm)
{
    mode = cm;
}

qreal Camera::getEyeSeparation() const
{
    return eyeSeparation;
}

void Camera::setEyeSeparation(qreal es)
{
    eyeSeparation = es;
}

Projection Camera::getProjectionType() const
{
    return session->cameraBuffer[id].ptype;
}

void Camera::setProjectionType(Projection p)
{
    if (session->cameraBuffer[id].ptype != p)
    {
        session->cameraBuffer[id].ptype = p;

        emit projectionChanged(updateProjection());
        emit projectionTypeChanged(p);
    }
}

qreal Camera::getZoom() const
{
    return zoom;
}

void Camera::setZoom(qreal z)
{
    zoom = z;
    emit projectionChanged(updateProjection());
}
