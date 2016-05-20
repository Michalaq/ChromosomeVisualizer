#include "attributes.h"
#include "ui_attributes.h"

Attributes::Attributes(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Attributes)
{
    ui->setupUi(this);
}

Attributes::~Attributes()
{
    delete ui;
}

void Attributes::handleSelection(const AtomSelection &selection)
{
    if (!selection.atomCount())
        return ui->tabWidget->hide();

    ui->tabWidget->show();
}
