#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "../QtChromosomeViz_v2/SelectionOperationsWidget.hpp"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    lastFrame(0)//TODO być może wywalić
{
    ui->setupUi(this);

    ui->move->load(":/move");
    ui->rotate->load(":/rotate");
    ui->scale->load(":/scale");

    auto * panel = new SelectionOperationsWidget(ui->scene);
    panel->show();
}

MainWindow::~MainWindow()
{
    delete ui;
}

#include "../QtChromosomeViz_v2/bartekm_code/PDBSimulation.h"
#include "../QtChromosomeViz_v2/bartekm_code/NullSimulation.h"

std::shared_ptr<Simulation> tmp;//TODO paskudny hack, usunąć po dodaniu wątku
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

    ui->horizontalSlider->setMaximum(0);
    ui->horizontalSlider->setValue(0);

    ui->spinBox->setMaximum(0);
    ui->spinBox->setValue(0);

    lastFrame = 0;

    connect(simulation.get(), SIGNAL(frameCountChanged(int)), this, SLOT(updateFrameCount(int)));
    tmp=simulation;//TODO paskudny hack, usunąć po dodaniu wątku
    simulation->getFrame(10);//TODO paskudny hack, usunąć po dodaniu wątku
}

void MainWindow::updateFrameCount(int n)
{
    lastFrame = n - 1;

    ui->horizontalSlider->setMaximum(lastFrame);
    ui->spinBox->setMaximum(lastFrame);
    ui->plot->setMaximum(lastFrame);
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
        n = lastFrame;

    ui->horizontalSlider->setValue(n);
    ui->spinBox->setValue(n);
    ui->scene->setFrame(n);
}

void MainWindow::pb(bool f)
{
    if (f)
    {
        if (ui->pushButton_4->isChecked())
            ui->pushButton_4->click();

        ui->pushButton_3->setText("||");
        connect(&timer, SIGNAL(timeout()), this, SLOT(ab()));
        timer.start();
    }
    else
    {
        ui->pushButton_3->setText("<");
        timer.stop();
        timer.disconnect();
    }
}

void MainWindow::pf(bool f)
{
    if (f)
    {
        if (ui->pushButton_3->isChecked())
            ui->pushButton_3->click();

        ui->pushButton_4->setText("||");
        connect(&timer, SIGNAL(timeout()), this, SLOT(af()));
        timer.start();
    }
    else
    {
        ui->pushButton_4->setText(">");
        timer.stop();
        timer.disconnect();
    }
}

void MainWindow::af()
{
    int n = ui->spinBox->value() + 1;

    if (n > lastFrame)
        n = 0;

    ui->horizontalSlider->setValue(n);
    ui->spinBox->setValue(n);
    ui->scene->setFrame(n);
    tmp->getFrame(n+1);//TODO paskudny hack, usunąć po dodaniu wątku
}

void MainWindow::ff()
{
    int n = lastFrame;

    ui->horizontalSlider->setValue(n);
    ui->spinBox->setValue(n);
    ui->scene->setFrame(n);
}
