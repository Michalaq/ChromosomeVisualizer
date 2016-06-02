#include "viewport.h"
#include "ui_viewport.h"

Viewport::Viewport(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Viewport)
{
    ui->setupUi(this);
}

Viewport::~Viewport()
{
    delete ui;
}
