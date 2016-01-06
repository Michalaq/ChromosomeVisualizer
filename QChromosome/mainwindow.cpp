#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    frameCount(0)
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
#include "../QtChromosomeViz_v2/bartekm_code/NullSimulation.h"

void MainWindow::openSimulation()
{
    QString path = QFileDialog::getOpenFileName(this, "", "/home", "RCSB Protein Data Bank (*.pdb)");

    std::shared_ptr<Simulation> simulation;

    if (!path.isEmpty())
        simulation = std::make_shared<PDBSimulation>(path.toStdString());
    else
        simulation = std::make_shared<NullSimulation>();

    ui->scene->setSimulation(simulation);
    ui->plot->setSimulation(simulation);

    frameCount = simulation->getFrameCount();

    ui->horizontalSlider->setMaximum(frameCount - 1);
    ui->horizontalSlider->setValue(0);

    ui->spinBox->setMaximum(frameCount - 1);
    ui->spinBox->setValue(0);
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
        n = frameCount - 1;

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

    if (n == frameCount)
        n = 0;

    ui->horizontalSlider->setValue(n);
    ui->spinBox->setValue(n);
    ui->scene->setFrame(n);
}

void MainWindow::ff()
{
    int n = frameCount;

    ui->horizontalSlider->setValue(n);
    ui->spinBox->setValue(n);
    ui->scene->setFrame(n);
}
