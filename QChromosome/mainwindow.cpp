#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "../QtChromosomeViz_v2/SelectionOperationsWidget.hpp"//TODO do wywalenia po zaimplementowaniu widgeta
#include "../QtChromosomeViz_v2/DisplayParametersWidget.hpp"
#include "../QtChromosomeViz_v2/bartekm_code/PDBSimulationLayer.h"
#include <QKeyEvent>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    simulation(),
    currentFrame(0),//TODO być może wywalić, jak ukryje się suwaki, gdy jest plik jednoklatkowy
    lastFrame(0),//TODO być może wywalić, jak ukryje się suwaki, gdy jest plik jednoklatkowy
    actionGroup(new QActionGroup(this))
{
    setCorner(Qt::TopLeftCorner, Qt::LeftDockWidgetArea);
    setCorner(Qt::TopRightCorner, Qt::RightDockWidgetArea);
    setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);
    setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);

    ui->setupUi(this);

    ui->tabWidget->hide();

    actionGroup->addAction(ui->actionSelect);
    actionGroup->addAction(ui->actionMove);
    actionGroup->addAction(ui->actionRotate);
    actionGroup->addAction(ui->actionScale);

    bindings.insert(Qt::Key_Q, ui->actionMove);
    bindings.insert(Qt::Key_W, ui->actionRotate);
    bindings.insert(Qt::Key_E, ui->actionScale);

    modifiers.push_back(ui->actionMove);

    //TODO do wywalenia po zaimplementowaniu widgeta
    auto x = new SelectionOperationsWidget(ui->tab);
    x->setVizWidget(ui->scene);
    x->setStyleSheet("SelectionOperationsWidget>QLabel { color: #d9d9d9; }");

    auto y = new DisplayParametersWidget(ui->dockWidget);
    y->setVizWidget(ui->scene);
    y->setStyleSheet("DisplayParametersWidget>QLabel { color: #d9d9d9; }");
    auto boxLayout = new QVBoxLayout();
    boxLayout->addWidget(y);
    ui->dockWidgetContents->setLayout(boxLayout);

    connect(ui->horizontalSlider_2, &RangeSlider::lowerBoundChanged, ui->spinBox, &SpinBox::setMinimum);
    connect(ui->horizontalSlider_2, &RangeSlider::lowerBoundChanged, ui->spinBox_3, &SpinBox::setMinimum);
    connect(ui->horizontalSlider_2, &RangeSlider::lowerBoundChanged, ui->horizontalSlider, &QSlider::setMinimum);
    connect(ui->horizontalSlider_2, &RangeSlider::lowerBoundChanged, ui->plot, &Plot::setMinimum);

    connect(ui->horizontalSlider_2, &RangeSlider::upperBoundChanged, ui->spinBox, &SpinBox::setMaximum);
    connect(ui->horizontalSlider_2, &RangeSlider::upperBoundChanged, ui->spinBox_2, &SpinBox::setMaximum);
    connect(ui->horizontalSlider_2, &RangeSlider::upperBoundChanged, ui->horizontalSlider, &QSlider::setMaximum);
    connect(ui->horizontalSlider_2, &RangeSlider::upperBoundChanged, ui->plot, &Plot::setMaximum);

    connect(ui->actionMove, SIGNAL(toggled(bool)), this, SLOT(move(bool)));
    connect(ui->actionRotate, SIGNAL(toggled(bool)), this, SLOT(rotate(bool)));
    connect(ui->actionScale, SIGNAL(toggled(bool)), this, SLOT(scale(bool)));
    move(true);
}

MainWindow::~MainWindow()
{
    delete ui;
}



void MainWindow::openSimulation()
{
    QString path = QFileDialog::getOpenFileName(this, "", "/home", "RCSB Protein Data Bank (*.pdb)");

    if (!path.isEmpty())
    {//TODO tu może być problem z synchronizacją i gubieniem sygnału
        QObject::disconnect(this, SLOT(updateFrameCount(int)));

        auto simulationLayer = std::make_shared<PDBSimulationLayer>(path.toStdString());
        if (!simulation)
            simulation = std::make_shared<Simulation>();

        simulation->addSimulationLayer(simulationLayer);
        ui->scene->setSimulation(simulation);
        ui->plot->setSimulation(simulation);

        ui->horizontalSlider->setMaximum(0);
        ui->horizontalSlider_2->setMaximum(0);

        ui->spinBox->setMaximum(0);
        ui->spinBox_2->setMaximum(0);
        ui->spinBox_3->setMaximum(0);

        lastFrame = 0;//TODO być może do wywalenia

        connect(simulation.get(), SIGNAL(frameCountChanged(int)), this, SLOT(updateFrameCount(int)));
        simulation->getFrame(10);//TODO paskudny hack, usunąć po dodaniu wątku
    }
}

void MainWindow::updateFrameCount(int n)
{
    lastFrame = n - 1;

    ui->horizontalSlider_2->setMaximum(lastFrame);
    ui->spinBox_3->setMaximum(lastFrame);

    ui->horizontalSlider_2->setUpperBound(lastFrame);
}

void MainWindow::setFrame(int n)
{
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
    if (currentFrame != 0)
        setFrame(--currentFrame);
    else
        if (ui->reverse->isChecked())
            ui->reverse->click();
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
    simulation->getFrame(currentFrame+1);//TODO paskudny hack, usunąć po dodaniu wątku
    if (currentFrame != lastFrame)
        setFrame(++currentFrame);
    else
        if (ui->play->isChecked())
            ui->play->click();
}

void MainWindow::end()
{
    setFrame(lastFrame);
    simulation->getFrame(lastFrame+1);//TODO paskudny hack, usunąć po dodaniu wątku
}

void MainWindow::selectAll()
{
    /*QList<unsigned int> all;

    size_t count = simulation->getFrame(currentFrame)->atoms.size();

    for (unsigned int i = 0; i < count; i++)
        all.push_back(i);

    //ui->scene->setVisibleSelection(all);*/
}

void MainWindow::handleSelection(const AtomSelection &selection)
{
    ui->camera->setOrigin(selection.weightCenter());

    if (selection.atomCount())
        ui->tabWidget->show();
    else
        ui->tabWidget->hide();
}

void MainWindow::setBaseAction(bool enabled)
{
    if (enabled)
        modifiers.last() = qobject_cast<QAction*>(sender());
}

void MainWindow::move(bool checked)
{
    if (checked)
        connect(ui->camera, SIGNAL(delta(int,int)), ui->camera, SLOT(move(int,int)));
    else
        ui->camera->disconnect(ui->camera);
}

void MainWindow::rotate(bool checked)
{
    if (checked)
        connect(ui->camera, SIGNAL(delta(int,int)), ui->camera, SLOT(rotate(int,int)));
    else
        ui->camera->disconnect(ui->camera);
}

void MainWindow::scale(bool checked)
{
    if (checked)
        connect(ui->camera, SIGNAL(delta(int,int)), ui->camera, SLOT(scale(int,int)));
    else
        ui->camera->disconnect(ui->camera);
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->isAutoRepeat())
        return event->ignore();

    auto i = bindings.find(static_cast<Qt::Key>(event->key()));

    if (i != bindings.end())
    {
        lookup[i.key()] = modifiers.insert(modifiers.end(), i.value());
        actionGroup->setDisabled(true);
        i.value()->setChecked(true);
    }

    QMainWindow::keyPressEvent(event);
}

void MainWindow::keyReleaseEvent(QKeyEvent *event)
{
    if (event->isAutoRepeat())
        return event->ignore();

    auto i = lookup.find(static_cast<Qt::Key>(event->key()));

    if (i != lookup.end())
    {
        modifiers.erase(i.value());
        modifiers.last()->setChecked(true);

        lookup.erase(i);

        if (lookup.isEmpty())
            actionGroup->setEnabled(true);
    }

    QMainWindow::keyReleaseEvent(event);
}
