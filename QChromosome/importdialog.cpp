#include "importdialog.h"
#include "ui_importdialog.h"

#include "../QtChromosomeViz_v2/bartekm_code/SimulationLayer.h"

ImportDialog::ImportDialog(SimulationLayer *sl, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ImportDialog),
    simulationLayer(sl)
{
    ui->setupUi(this);
    setFixedSize(width(), minimumHeight());

    ui->spinBox->setMaximum(simulationLayer->last < simulationLayer->first ? 2147483647 : simulationLayer->last);
    ui->spinBox_2->setMinimum(simulationLayer->first - 1);

    ui->spinBox->setValue(simulationLayer->first, false);
    ui->spinBox_2->setValue(simulationLayer->last, false);
    ui->spinBox_3->setValue(simulationLayer->stride, false);

    connect(ui->spinBox, QOverload<int>::of(&SpinBox::valueChanged), [this](int value) {
        simulationLayer->first = value;
        ui->spinBox_2->setMinimum(value - 1);
    });

    connect(ui->spinBox_2, QOverload<int>::of(&SpinBox::valueChanged), [this](int value) {
        simulationLayer->last = value;
        ui->spinBox->setMaximum(value < simulationLayer->first ? 2147483647 : simulationLayer->last);
    });

    connect(ui->spinBox_3, QOverload<int>::of(&SpinBox::valueChanged), [this](int value) {
        simulationLayer->stride = value;
    });
}

ImportDialog::~ImportDialog()
{
    delete ui;
}
