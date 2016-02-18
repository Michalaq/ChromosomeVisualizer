#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "../QtChromosomeViz_v2/SelectionOperationsWidget.hpp"//TODO do wywalenia po zaimplementowaniu widgeta

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    currentFrame(0),//TODO być może wywalić, jak ukryje się suwaki, gdy jest plik jednoklatkowy
    lastFrame(0)//TODO być może wywalić, jak ukryje się suwaki, gdy jest plik jednoklatkowy
{
    setCorner(Qt::TopLeftCorner, Qt::LeftDockWidgetArea);
    setCorner(Qt::TopRightCorner, Qt::RightDockWidgetArea);
    setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);
    setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);

    ui->setupUi(this);

    cacheProperties(this, {});

    auto x = new SelectionOperationsWidget(ui->tab);
    x->setVizWidget(ui->scene);
    x->setStyleSheet("QLabel { color: #d9d9d9; }");
}

MainWindow::~MainWindow()
{
    delete ui;
}

#include <QMouseEvent>
#include "draggable.h"

bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{
    if (Draggable::pressedButton() != Qt::NoButton)
    {
        if (event->type() == QEvent::MouseButtonPress && event->spontaneous())
            return true;

        if (event->type() == QEvent::MouseButtonRelease && reinterpret_cast<QMouseEvent*>(event)->button() != Draggable::pressedButton())
            return true;
    }

    return QMainWindow::eventFilter(watched, event);
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

void MainWindow::setFrame(int n)
{
    Q_ASSERT(0 <= n && n <= lastFrame);

    currentFrame = n;

    ui->horizontalSlider->setValue(n);
    ui->spinBox->setValue(n);
    ui->scene->setFrame(n);
    ui->plot->setFrame(n);
}

void MainWindow::start()
{
    setFrame(0);
}

void MainWindow::previous()
{
    setFrame(currentFrame != 0 ? currentFrame - 1 : lastFrame);
}

void MainWindow::reverse(bool checked)
{
    if (checked)
    {
        if (ui->play->isChecked())
            ui->play->click();

        connect(&timer, SIGNAL(timeout()), this, SLOT(previous()));
        timer.start();
    }
    else
    {
        timer.stop();
        timer.disconnect();
    }
}

void MainWindow::play(bool checked)
{
    if (checked)
    {
        if (ui->reverse->isChecked())
            ui->reverse->click();

        connect(&timer, SIGNAL(timeout()), this, SLOT(next()));
        timer.start();
    }
    else
    {
        timer.stop();
        timer.disconnect();
    }
}

void MainWindow::next()
{
    setFrame(currentFrame != lastFrame ? currentFrame + 1 : 0);
    tmp->getFrame(currentFrame+1);//TODO paskudny hack, usunąć po dodaniu wątku
}

void MainWindow::end()
{
    setFrame(lastFrame);
    tmp->getFrame(lastFrame+1);//TODO paskudny hack, usunąć po dodaniu wątku
}

void MainWindow::handleSelection(const AtomSelection &selection)
{
    ui->camera->setOrigin(selection.weightCenter());
}

void MainWindow::cacheProperties(QWidget *widget, QHash<QString, QHash<QString, QHash<QString, QVariant> > > cache)
{
    for (auto child : widget->children())
        if (auto tmp = qobject_cast<QWidget*>(child))
            cacheProperties(tmp, cache);
}
