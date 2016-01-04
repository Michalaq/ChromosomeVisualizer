#include "display.h"
#include "ui_display.h"

Display::Display(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Display)
{
    ui->setupUi(this);

    ui->move->load(":/move");
    ui->rotate->load(":/rotate");
    ui->scale->load(":/scale");
}

Display::~Display()
{
    delete ui;
}
