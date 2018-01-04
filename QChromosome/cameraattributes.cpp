#include "cameraattributes.h"
#include "ui_cameraattributes.h"

CameraAttributes::CameraAttributes(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CameraAttributes)
{
    ui->setupUi(this);
}

CameraAttributes::~CameraAttributes()
{
    delete ui;
}
