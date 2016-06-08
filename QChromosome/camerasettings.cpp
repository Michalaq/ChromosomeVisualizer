#include "camerasettings.h"
#include "ui_camerasettings.h"

CameraSettings::CameraSettings(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CameraSettings),
    camera(nullptr)
{
    ui->setupUi(this);

    // coordinates
    connect(ui->doubleSpinBox_7, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [this](double value) {
        camera->setPosition(QVector3D(value, ui->doubleSpinBox_8->value(), ui->doubleSpinBox_9->value()));
    });

    connect(ui->doubleSpinBox_8, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [this](double value) {
        camera->setPosition(QVector3D(ui->doubleSpinBox_7->value(), value, ui->doubleSpinBox_9->value()));
    });

    connect(ui->doubleSpinBox_9, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [this](double value) {
        camera->setPosition(QVector3D(ui->doubleSpinBox_7->value(), ui->doubleSpinBox_8->value(), value));
    });

    // Euler angles
}

CameraSettings::~CameraSettings()
{
    delete ui;
}

void CameraSettings::setCamera(Camera *c)
{
    camera = c;
}
