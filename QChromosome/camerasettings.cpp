#include "camerasettings.h"
#include "ui_camerasettings.h"
#include "camera.h"

CameraSettings::CameraSettings(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CameraSettings),
    camera(nullptr)
{
    ui->setupUi(this);

    // coordinates
    connect(ui->doubleSpinBox_7, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [this](double val) {
        auto pos = camera->eye;
        pos.setX(val);
        camera->setPosition(pos);
    });

    connect(ui->doubleSpinBox_8, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [this](double val) {
        auto pos = camera->eye;
        pos.setY(val);
        camera->setPosition(pos);
    });

    connect(ui->doubleSpinBox_9, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [this](double val) {
        auto pos = camera->eye;
        pos.setZ(val);
        camera->setPosition(pos);
    });

    // angles
    connect(ui->doubleSpinBox_10, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [this](double val) {
        camera->setEulerAgnles(val, camera->p, camera->b);
    });

    connect(ui->doubleSpinBox_11, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [this](double val) {
        camera->setEulerAgnles(camera->h, val, camera->b);
    });

    connect(ui->doubleSpinBox_12, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [this](double val) {
        camera->setEulerAgnles(camera->h, camera->p, val);
    });

    // focal length
    connect(ui->doubleSpinBox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [this](double val) {
        camera->setFocalLength(val);
    });

    // aperture width
    connect(ui->doubleSpinBox_2, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [this](double val) {
        camera->setApertureWidth(val);
    });

    // field of view
    connect(ui->doubleSpinBox_3, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [this](double val) {
        camera->setFieldOfView(val);
    });

    // rotation type
    connect(ui->comboBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [this](int index) {
        camera->setRotationType(index);
    });

    // near clipping
    connect(ui->doubleSpinBox_5,static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [this](double val) {
        camera->setNearClipping(val);
    });

    // far clipping
    connect(ui->doubleSpinBox_6, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [this](double val) {
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
    camera = static_cast<CameraItem*>(selection.first().internalPointer())->getCamera();

    // coordinates
    ui->doubleSpinBox_7->setValue(camera->eye.x(), false);
    ui->doubleSpinBox_8->setValue(camera->eye.y(), false);
    ui->doubleSpinBox_9->setValue(camera->eye.z(), false);

    // angles
    ui->doubleSpinBox_10->setValue(camera->h, false);
    ui->doubleSpinBox_11->setValue(camera->p, false);
    ui->doubleSpinBox_12->setValue(camera->b, false);

    // focal length
    ui->doubleSpinBox->setValue(camera->focalLength, false);

    // aperture width
    ui->doubleSpinBox_2->setValue(camera->apertureWidth, false);

    // field of view
    ui->doubleSpinBox_3->setValue((qreal)2.f * qRadiansToDegrees(qAtan(camera->apertureWidth / 2 / camera->focalLength)), false);

    // rotation type
    ui->comboBox->setCurrentIndex(camera->rotationType, false);

    // near clipping
    ui->doubleSpinBox_5->setValue(camera->nearClipping, false);

    // far clipping
    ui->doubleSpinBox_6->setValue(camera->farClipping, false);

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
{
    // coordinates
    ui->doubleSpinBox_7->setValue(camera->eye.x(), false);
    ui->doubleSpinBox_8->setValue(camera->eye.y(), false);
    ui->doubleSpinBox_9->setValue(camera->eye.z(), false);

    // angles
    ui->doubleSpinBox_10->setValue(camera->h, false);
    ui->doubleSpinBox_11->setValue(camera->p, false);
    ui->doubleSpinBox_12->setValue(camera->b, false);
}
