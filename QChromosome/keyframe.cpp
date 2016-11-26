#include "keyframe.h"
#include "ui_keyframe.h"

Keyframes::Keyframes(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Keyframes)
{
    ui->setupUi(this);
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
}
