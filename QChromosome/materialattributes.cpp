#include "materialattributes.h"
#include "ui_materialattributes.h"

MaterialAttributes::MaterialAttributes(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MaterialAttributes)
{
    ui->setupUi(this);
}

MaterialAttributes::~MaterialAttributes()
{
    delete ui;
}

void MaterialAttributes::handleSelection(const QList<Material *> &selected)
{
    if (selected.isEmpty())
        return hide();

    //

    show();
}
