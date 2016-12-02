#include "camerasettings.h"
#include "ui_camerasettings.h"

CameraSettings::CameraSettings(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CameraSettings),
    camera(nullptr)
{
    ui->setupUi(this);

    // coordinates
    connect(ui->doubleSpinBox_7, &QDoubleSpinBox::editingFinished, [this]() {
        camera->setPosition(QVector3D(ui->doubleSpinBox_7->value(), ui->doubleSpinBox_8->value(), ui->doubleSpinBox_9->value()));
    });

    connect(ui->doubleSpinBox_8, &QDoubleSpinBox::editingFinished, [this]() {
        camera->setPosition(QVector3D(ui->doubleSpinBox_7->value(), ui->doubleSpinBox_8->value(), ui->doubleSpinBox_9->value()));
    });

    connect(ui->doubleSpinBox_9, &QDoubleSpinBox::editingFinished, [this]() {
        camera->setPosition(QVector3D(ui->doubleSpinBox_7->value(), ui->doubleSpinBox_8->value(), ui->doubleSpinBox_9->value()));
    });

    // angles
    connect(ui->doubleSpinBox_10, &QDoubleSpinBox::editingFinished, [this]() {
        camera->setEulerAgnles(ui->doubleSpinBox_10->value(), ui->doubleSpinBox_11->value(), ui->doubleSpinBox_12->value());
    });

    connect(ui->doubleSpinBox_11, &QDoubleSpinBox::editingFinished, [this]() {
        camera->setEulerAgnles(ui->doubleSpinBox_10->value(), ui->doubleSpinBox_11->value(), ui->doubleSpinBox_12->value());
    });

    connect(ui->doubleSpinBox_12, &QDoubleSpinBox::editingFinished, [this]() {
        camera->setEulerAgnles(ui->doubleSpinBox_10->value(), ui->doubleSpinBox_11->value(), ui->doubleSpinBox_12->value());
    });

    // focal length
    connect(ui->doubleSpinBox, &QDoubleSpinBox::editingFinished, [this]() {
        camera->setFocalLength(ui->doubleSpinBox->value());
    });

    // aperture width
    connect(ui->doubleSpinBox_2, &QDoubleSpinBox::editingFinished, [this]() {
        camera->setApertureWidth(ui->doubleSpinBox_2->value());
    });

    // field of view
    connect(ui->doubleSpinBox_3, &QDoubleSpinBox::editingFinished, [this]() {
        camera->setFieldOfView(ui->doubleSpinBox_3->value());
    });

    // rotation type
    connect(ui->comboBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [this](int index) {
        camera->setRotationType(index);
    });

    // near clipping
    connect(ui->doubleSpinBox_5, &QDoubleSpinBox::editingFinished, [this]() {
        camera->setNearClipping(ui->doubleSpinBox_5->value());
    });

    // far clipping
    connect(ui->doubleSpinBox_6, &QDoubleSpinBox::editingFinished, [this]() {
        camera->setFarClipping(ui->doubleSpinBox_6->value());
    });
}

CameraSettings::~CameraSettings()
{
    delete ui;
}

#include <QtMath>

void CameraSettings::setCamera(Camera *c)
{
    if (camera)
        disconnect(camera, &Camera::modelViewChanged, this, &CameraSettings::updateModelView);

    camera = c;

    // coordinates
    ui->doubleSpinBox_7->setValue(camera->eye.x());
    ui->doubleSpinBox_8->setValue(camera->eye.y());
    ui->doubleSpinBox_9->setValue(camera->eye.z());

    // angles
    ui->doubleSpinBox_10->setValue(camera->h);
    ui->doubleSpinBox_11->setValue(camera->p);
    ui->doubleSpinBox_12->setValue(camera->b);

    // focal length
    ui->doubleSpinBox->setValue(camera->focalLength);

    // aperture width
    ui->doubleSpinBox_2->setValue(camera->apertureWidth);

    // field of view
    ui->doubleSpinBox_3->setValue((qreal)2.f * qRadiansToDegrees(qAtan(camera->apertureWidth / 2 / camera->focalLength)));

    // rotation type
    ui->comboBox->setCurrentIndex(camera->rotationType);

    // near clipping
    ui->doubleSpinBox_5->setValue(camera->nearClipping);

    // far clipping
    ui->doubleSpinBox_6->setValue(camera->farClipping);

    connect(camera, &Camera::modelViewChanged, this, &CameraSettings::updateModelView);

    connect(camera, &Camera::projectionChanged, [this] {
        // focal length
        ui->doubleSpinBox->setValue(camera->focalLength);

        // aperture width
        ui->doubleSpinBox_2->setValue(camera->apertureWidth);

        // field of view
        ui->doubleSpinBox_3->setValue((qreal)2.f * qRadiansToDegrees(qAtan(camera->apertureWidth / 2 / camera->focalLength)));
    });
}

void CameraSettings::setRotationType(int rt)
{
    ui->comboBox->setCurrentIndex(rt);
}

void CameraSettings::updateModelView()
{
    // coordinates
    ui->doubleSpinBox_7->setValue(camera->eye.x());
    ui->doubleSpinBox_8->setValue(camera->eye.y());
    ui->doubleSpinBox_9->setValue(camera->eye.z());

    // angles
    ui->doubleSpinBox_10->setValue(camera->h);
    ui->doubleSpinBox_11->setValue(camera->p);
    ui->doubleSpinBox_12->setValue(camera->b);
}
