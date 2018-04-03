#include "importdialog.h"
#include "ui_importdialog.h"

#include "../QtChromosomeViz_v2/bartekm_code/SimulationLayer.h"

ImportDialog::ImportDialog(SimulationLayer *sl, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ImportDialog),
    simulationLayer(sl)
{
    ui->setupUi(this);
}

ImportDialog::~ImportDialog()
{
    delete ui;
}
