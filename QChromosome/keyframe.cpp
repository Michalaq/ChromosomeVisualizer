#include "keyframe.h"
#include "ui_keyframe.h"

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

    connect(ui->spinBox, &QSpinBox::editingFinished, [this] {
        ip->changeKey(ui->spinBox->value());
    });

    connect(ui->checkBox, &QCheckBox::toggled, [this](bool b) {
        ip->lockKey(b);
        ui->spinBox->setEnabled(!b);
    });

    connect(ui->checkBox_2, &QCheckBox::toggled, [this](bool b) {
        ip->lockValue(b);
        ui->doubleSpinBox->setEnabled(!b);
    });
}

Keyframes::~Keyframes()
{
    delete ui;
}

void Keyframes::setInterpolator(Interpolator *_ip)
{
    ip = _ip;
}

void Keyframes::updateContents()
{
    ui->spinBox->setValue(ip->selectedKeyframe());
    ui->checkBox->setChecked(ip->isKeyLocked());
    ui->checkBox_2->setChecked(ip->isValueLocked(ip->selectedKeyframe()));
}
