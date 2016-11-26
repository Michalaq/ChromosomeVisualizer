#include "keyframe.h"
#include "ui_keyframe.h"

Keyframes::Keyframes(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Keyframes)
{
    ui->setupUi(this);

    connect(ui->spinBox, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), [this](int sv) {
        auto v = ip->frame.value();
        ip->keyframes.erase(ip->frame);
        ip->frame = ip->keyframes.insertMulti(sv, v);
        ip->updateCurves();
    });

    connect(ui->checkBox, &QCheckBox::toggled, [this](bool b) {
        ip->locked[ip->frame.key()] = b;
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
    ui->spinBox->setValue(ip->frame.key());
    ui->checkBox->setChecked(ip->locked[ip->frame.key()]);
}
