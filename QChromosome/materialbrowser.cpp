#include "materialbrowser.h"
#include "ui_materialbrowser.h"

MaterialBrowser::MaterialBrowser(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MaterialBrowser)
{
    ui->setupUi(this);
}

MaterialBrowser::~MaterialBrowser()
{
    delete ui;
}
