#include "keyframe.h"
#include "ui_keyframe.h"

Keyframes::Keyframes(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Keyframes)
{
    ui->setupUi(this);

    connect(ui->spinBox, &QSpinBox::editingFinished, [this] {
        ip->changeKey(ui->spinBox->value());
    });

    connect(ui->checkBox, &QCheckBox::toggled, [this](bool b) {
        ip->lockKey(b);
        ui->spinBox->setEnabled(!b);
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
    ui->checkBox->setChecked(ip->isKeyLocked(ip->selectedKeyframe()));
}
