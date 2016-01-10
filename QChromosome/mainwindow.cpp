#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "../QtChromosomeViz_v2/SelectionOperationsWidget.hpp"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    currentFrame(0),//TODO być może wywalić, jak ukryje się suwaki, gdy jest plik jednoklatkowy
    lastFrame(0)//TODO być może wywalić, jak ukryje się suwaki, gdy jest plik jednoklatkowy
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
    {//TODO tu może być problem z synchronizacją i gubieniem sygnału
        QObject::disconnect(this, SLOT(updateFrameCount(int)));

        simulation = std::make_shared<PDBSimulation>(path.toStdString());

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
}

void MainWindow::updateFrameCount(int n)
{
    lastFrame = n - 1;

    ui->horizontalSlider->setMaximum(lastFrame);
    ui->spinBox->setMaximum(lastFrame);
    ui->plot->setMaximum(lastFrame);
}

void MainWindow::start()
{
    ui->horizontalSlider->setValue(0);
    ui->spinBox->setValue(0);
    ui->scene->setFrame(0);
}

void MainWindow::previous()
{
    if (--currentFrame < 0)
        currentFrame = lastFrame;

    ui->horizontalSlider->setValue(currentFrame);
    ui->spinBox->setValue(currentFrame);
    ui->scene->setFrame(currentFrame);
}

void MainWindow::reverse(bool checked)
{
    if (checked)
    {
        if (ui->play->isChecked())
            ui->play->click();

        ui->reverse->setText("||");

        connect(&timer, SIGNAL(timeout()), this, SLOT(previous()));
        timer.start();
    }
    else
    {
        timer.stop();
        timer.disconnect();

        ui->reverse->setText("<");
    }
}

void MainWindow::play(bool checked)
{
    if (checked)
    {
        if (ui->reverse->isChecked())
            ui->reverse->click();

        ui->play->setText("||");

        connect(&timer, SIGNAL(timeout()), this, SLOT(next()));
        timer.start();
    }
    else
    {
        timer.stop();
        timer.disconnect();

        ui->play->setText(">");
    }
}

void MainWindow::next()
{
    if (++currentFrame > lastFrame)
        currentFrame = 0;

    ui->horizontalSlider->setValue(currentFrame);
    ui->spinBox->setValue(currentFrame);
    ui->scene->setFrame(currentFrame);
    tmp->getFrame(currentFrame+1);//TODO paskudny hack, usunąć po dodaniu wątku
}

void MainWindow::end()
{
    int n = lastFrame;

    ui->horizontalSlider->setValue(n);
    ui->spinBox->setValue(n);
    ui->scene->setFrame(n);
    tmp->getFrame(n+1);//TODO paskudny hack, usunąć po dodaniu wątku
}
