#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->move->load(":/move");
    ui->rotate->load(":/rotate");
    ui->scale->load(":/scale");
}

MainWindow::~MainWindow()
{
    delete ui;
}

#include "../QtChromosomeViz_v2/bartekm_code/PDBSimulation.h"

void MainWindow::openSimulation()
{
    QString path = QFileDialog::getOpenFileName(this, "", "/home", "RCSB Protein Data Bank (*.pdb)");

    if (!path.isEmpty())
    {
        auto simulation = std::make_shared<PDBSimulation>(path.toStdString());

        ui->scene->setSimulation(simulation);

        ui->horizontalSlider->setValue(0);
        ui->spinBox->setValue(0);
    }
}
