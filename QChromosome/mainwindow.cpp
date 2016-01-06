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

    openSimulation(); //TODO do wywalenia
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
        ui->plot->setSimulation(simulation);

        ui->horizontalSlider->setMaximum(100);//simulation_->getFrameCount() - 1
        ui->horizontalSlider->setValue(0);

        ui->spinBox->setMaximum(100);//simulation_->getFrameCount() - 1
        ui->spinBox->setValue(0);
    }
}

void MainWindow::bb()
{
    ui->horizontalSlider->setValue(0);
    ui->spinBox->setValue(0);
    ui->scene->setFrame(0);
}

void MainWindow::ab()
{
    int n = ui->spinBox->value() - 1;

    if (n < 0)
        n = ui->spinBox->maximum();

    ui->horizontalSlider->setValue(n);
    ui->spinBox->setValue(n);
    ui->scene->setFrame(n);
}

void MainWindow::pb()
{

}

void MainWindow::pf()
{

}

void MainWindow::af()
{
    int n = ui->spinBox->value() + 1;

    if (n > ui->spinBox->maximum())
        n = 0;

    ui->horizontalSlider->setValue(n);
    ui->spinBox->setValue(n);
    ui->scene->setFrame(n);
}

void MainWindow::ff()
{
    int n = ui->spinBox->maximum();

    ui->horizontalSlider->setValue(n);
    ui->spinBox->setValue(n);
    ui->scene->setFrame(n);
}
