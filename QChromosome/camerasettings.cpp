#include "camerasettings.h"
#include "ui_camerasettings.h"
#include "camera.h"

CameraSettings::CameraSettings(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CameraSettings)
{
    ui->setupUi(this);

    // coordinates
    connect(ui->doubleSpinBox_7, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [this](double val) {
        for (auto camera : cameras)
        {
            auto pos = camera->eye;
            pos.setX(val);
            camera->setPosition(pos);
        }
    });

    connect(ui->doubleSpinBox_8, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [this](double val) {
        for (auto camera : cameras)
        {
            auto pos = camera->eye;
            pos.setY(val);
            camera->setPosition(pos);
        }
    });

    connect(ui->doubleSpinBox_9, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [this](double val) {
        for (auto camera : cameras)
        {
            auto pos = camera->eye;
            pos.setZ(val);
            camera->setPosition(pos);
        }
    });

    // angles
    connect(ui->doubleSpinBox_10, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [this](double val) {
        for (auto camera : cameras)
            camera->setEulerAgnles(val, camera->p, camera->b);
    });

    connect(ui->doubleSpinBox_11, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [this](double val) {
        for (auto camera : cameras)
            camera->setEulerAgnles(camera->h, val, camera->b);
    });

    connect(ui->doubleSpinBox_12, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [this](double val) {
        for (auto camera : cameras)
            camera->setEulerAgnles(camera->h, camera->p, val);
    });

    // focal length
    connect(ui->doubleSpinBox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [this](double val) {
        for (auto camera : cameras)
            camera->setFocalLength(val);
    });

    // aperture width
    connect(ui->doubleSpinBox_2, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [this](double val) {
        for (auto camera : cameras)
            camera->setApertureWidth(val);
    });

    // field of view
    connect(ui->doubleSpinBox_3, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [this](double val) {
        for (auto camera : cameras)
            camera->setFieldOfView(val);
    });

    // rotation type
    connect(ui->comboBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [this](int index) {
        for (auto camera : cameras)
            camera->setRotationType(index);
    });

    // near clipping
    connect(ui->doubleSpinBox_5,static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [this](double val) {
        for (auto camera : cameras)
            camera->setNearClipping(val);
    });

    // far clipping
    connect(ui->doubleSpinBox_6, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [this](double val) {
        for (auto camera : cameras)
            camera->setFarClipping(val);
    });
}

CameraSettings::~CameraSettings()
{
    delete ui;
}

#include <QtMath>
#include "treeitem.h"

void CameraSettings::handleSelection(const QModelIndexList& selection)
{
    /*if (camera)
        disconnect(camera, &Camera::modelViewChanged, this, &CameraSettings::updateModelView);*/

    if (!selection.count())
        return hide();

    // set title
    title = "Camera Object ";

    if (selection.count() > 1)
        title += "(" + QString::number(selection.count()) + " elements) ";

    list.clear();

    for (auto i : selection)
        list += i.data().toString() + ", ";

    list.chop(2);

    ui->label_14->setText(title + "[" + ui->label_14->fontMetrics().elidedText(list, Qt::ElideRight, width() - ui->label_14->fontMetrics().width(title + "[]") - 58) + "]");

    // cameras
    cameras.clear();

    for (auto camera : selection)
        cameras.append(static_cast<CameraItem*>(camera.internalPointer())->getCamera());

    auto camera = cameras.first();

    // coordinates
    double x = camera->position().x();

    for (auto c : cameras)
        if (c->position().x() != x)
        {
            x = std::numeric_limits<double>::lowest();
            break;
        }
    ui->doubleSpinBox_7->setValue(x, false);

    double y = camera->position().y();

    for (auto c : cameras)
        if (c->position().y() != y)
        {
            y = std::numeric_limits<double>::lowest();
            break;
        }
    ui->doubleSpinBox_8->setValue(y, false);

    double z = camera->position().z();

    for (auto c : cameras)
        if (c->position().z() != z)
        {
            z = std::numeric_limits<double>::lowest();
            break;
        }
    ui->doubleSpinBox_9->setValue(z, false);

    // angles
    double h = camera->h;

    for (auto c : cameras)
        if (c->h != h)
        {
            h = std::numeric_limits<double>::lowest();
            break;
        }
    ui->doubleSpinBox_10->setValue(h, false);

    double p = camera->p;

    for (auto c : cameras)
        if (c->p != p)
        {
            p = std::numeric_limits<double>::lowest();
            break;
        }
    ui->doubleSpinBox_11->setValue(p, false);

    double b = camera->b;

    for (auto c : cameras)
        if (c->b != b)
        {
            b = std::numeric_limits<double>::lowest();
            break;
        }
    ui->doubleSpinBox_12->setValue(b, false);

    // focal length
    double focalLength = camera->focalLength;

    for (auto c : cameras)
        if (c->focalLength != focalLength)
        {
            focalLength = std::numeric_limits<double>::lowest();
            break;
        }
    ui->doubleSpinBox->setValue(focalLength, false);

    // aperture width
    double apertureWidth = camera->apertureWidth;

    for (auto c : cameras)
        if (c->apertureWidth != apertureWidth)
        {
            apertureWidth = std::numeric_limits<double>::lowest();
            break;
        }
    ui->doubleSpinBox_2->setValue(apertureWidth, false);

    // field of view
    ui->doubleSpinBox_3->setValue((qreal)2.f * qRadiansToDegrees(qAtan(camera->apertureWidth / 2 / camera->focalLength)), false);

    // TODO rotation type
    int rotationType = camera->rotationType;

    for (auto c : cameras)
        if (c->rotationType != rotationType)
        {
            rotationType = std::numeric_limits<int>::lowest();
            break;
        }
    ui->comboBox->setCurrentIndex(rotationType, false);

    // near clipping
    double nearClipping = camera->nearClipping;

    for (auto c : cameras)
        if (c->nearClipping != nearClipping)
        {
            nearClipping = std::numeric_limits<double>::lowest();
            break;
        }
    ui->doubleSpinBox_5->setValue(nearClipping, false);

    // far clipping
    double farClipping = camera->farClipping;

    for (auto c : cameras)
        if (c->farClipping != farClipping)
        {
            farClipping = std::numeric_limits<double>::lowest();
            break;
        }
    ui->doubleSpinBox_6->setValue(farClipping, false);

    //TODO
    /*connect(camera, &Camera::modelViewChanged, this, &CameraSettings::updateModelView);

    connect(camera, &Camera::projectionChanged, [this] {
        // focal length
        ui->doubleSpinBox->setValue(camera->focalLength, false);

        // aperture width
        ui->doubleSpinBox_2->setValue(camera->apertureWidth, false);

        // field of view
        ui->doubleSpinBox_3->setValue((qreal)2.f * qRadiansToDegrees(qAtan(camera->apertureWidth / 2 / camera->focalLength)), false);
    });*/
}

void CameraSettings::setRotationType(int rt)
{
    ui->comboBox->setCurrentIndex(rt);
}

void CameraSettings::resizeEvent(QResizeEvent *event)
{
    ui->label_14->setText(title + "[" + ui->label_14->fontMetrics().elidedText(list, Qt::ElideRight, width() - ui->label_14->fontMetrics().width(title + "[]") - 58) + "]");

    QWidget::resizeEvent(event);
}

void CameraSettings::updateModelView()
{/*
    // coordinates
    ui->doubleSpinBox_7->setValue(camera->eye.x(), false);
    ui->doubleSpinBox_8->setValue(camera->eye.y(), false);
    ui->doubleSpinBox_9->setValue(camera->eye.z(), false);

    // angles
    ui->doubleSpinBox_10->setValue(camera->h, false);
    ui->doubleSpinBox_11->setValue(camera->p, false);
    ui->doubleSpinBox_12->setValue(camera->b, false);*/
}
