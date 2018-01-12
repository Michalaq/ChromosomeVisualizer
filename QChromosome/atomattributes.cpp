#include "atomattributes.h"
#include "ui_atomattributes.h"

AtomAttributes::AtomAttributes(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AtomAttributes)
{
    ui->setupUi(this);
}

AtomAttributes::~AtomAttributes()
{
    delete ui;
}
