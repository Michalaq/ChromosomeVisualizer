#include "keyframe.h"
#include "ui_keyframe.h"
#include "interpolator.h"

Keyframes::Keyframes(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Keyframes)
{
    ui->setupUi(this);

    // ALERT hide unused controls
    ui->label_2->hide(); ui->formLayout_2->removeWidget(ui->label_2);
    ui->doubleSpinBox->hide(); ui->formLayout_2->removeWidget(ui->doubleSpinBox);
    ui->label_3->hide(); ui->formLayout_2->removeWidget(ui->label_3);
    ui->comboBox->hide(); ui->formLayout_2->removeWidget(ui->comboBox);

    connect(ui->spinBox, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), [this](int key) {
        interpolator->setKey(key);
    });

    connect(ui->checkBox, &QCheckBox::toggled, [this](bool b) {
        interpolator->lockTime(b);
        ui->spinBox->setEnabled(!b);
    });

    connect(ui->checkBox_2, &QCheckBox::toggled, [this](bool b) {
        interpolator->lockValue(b);
        ui->doubleSpinBox->setEnabled(!b);
    });
}

Keyframes::~Keyframes()
{
    delete ui;
}

void Keyframes::setSplineInterpolator(SplineInterpolator *si)
{
    interpolator = si;

    auto i = interpolator->selectedFrame();

    if (i == interpolator->constEnd())
        return hide();

    ui->spinBox->setValue(i.key(), false);
    ui->checkBox->setChecked(i.value().timeLocked());
    ui->checkBox_2->setChecked(i.value().valueLocked());

    show();
}
