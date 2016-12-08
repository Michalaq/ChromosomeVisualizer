#include "defaults.h"
#include "ui_defaults.h"

Defaults::Defaults(QWidget *parent) : QWidget(parent), ui(new Ui::Defaults)
{
    ui->setupUi(this);
}

Defaults::~Defaults()
{
    delete ui;
}
