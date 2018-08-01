#include "mediapanel.h"
#include "ui_mediapanel.h"

MediaPanel::MediaPanel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MediaPanel)
{
    ui->setupUi(this);
}

MediaPanel::~MediaPanel()
{
    delete ui;
}
