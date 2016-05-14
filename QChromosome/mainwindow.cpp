#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ui_projectsettings.h"

#include "../QtChromosomeViz_v2/bartekm_code/NullSimulation.h"
#include "../QtChromosomeViz_v2/SelectionOperationsWidget.hpp"//TODO do wywalenia po zaimplementowaniu widgeta
#include "../QtChromosomeViz_v2/DisplayParametersWidget.hpp"

SelectionOperationsWidget *z;//TODO paskudny hack, usunąć po zaimplementowaniu własnego widgeta
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    simulation(std::make_shared<NullSimulation>()),
    currentFrame(0),//TODO być może wywalić, jak ukryje się suwaki, gdy jest plik jednoklatkowy
    lastFrame(0),//TODO być może wywalić, jak ukryje się suwaki, gdy jest plik jednoklatkowy
    actionGroup(new QActionGroup(this)),
    renderSettings(new RenderSettings()),
    softMinimum(0),
    softMaximum(0)
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

    for (auto action : actionGroup->actions())
        connect(action, &QAction::toggled, [action, this] (bool checked) {
            ui->camera->setStatusTip(checked ? action->property("cameraStatusTip").toString() : "");

            if (!ui->statusBar->currentMessage().isEmpty())
                ui->statusBar->showMessage(action->property("cameraStatusTip").toString());
        });

    bindings.insert(Qt::Key_Q, ui->actionMove);
    bindings.insert(Qt::Key_W, ui->actionRotate);
    bindings.insert(Qt::Key_E, ui->actionScale);

    modifiers.push_back(ui->actionMove);

    //TODO do wywalenia po zaimplementowaniu widgeta
    z = new SelectionOperationsWidget(ui->scrollAreaWidgetContents);
    z->setVizWidget(ui->scene);
    z->setStyleSheet("SelectionOperationsWidget>QLabel { color: #d9d9d9; }");
    z->hide();

    auto y = new DisplayParametersWidget(ui->scrollAreaWidgetContents);
    y->setVizWidget(ui->scene);
    y->setStyleSheet("DisplayParametersWidget>QLabel { color: #d9d9d9; }");
    auto boxLayout = new QVBoxLayout();
    boxLayout->addWidget(y);
    boxLayout->addWidget(z);
    ui->scrollAreaWidgetContents->setLayout(boxLayout);
    // koniec

    connect(ui->spinBox_2, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), ui->horizontalSlider_2, &RangeSlider::setMinimum);
    connect(ui->spinBox_2, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), ui->spinBox_3, &SpinBox::setMinimum);
    connect(ui->spinBox_3, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), ui->horizontalSlider_2, &RangeSlider::setMaximum);
    connect(ui->spinBox_3, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), ui->spinBox_2, &SpinBox::setMaximum);

    /* connect actions */
    mappedSlot[ui->actionMove] = SLOT(move(int,int));
    mappedSlot[ui->actionRotate] = SLOT(rotate(int,int));
    mappedSlot[ui->actionScale] = SLOT(scale(int,int));

    ui->actionMove->toggle();

    connect(ui->actionSettings, SIGNAL(triggered(bool)), renderSettings, SLOT(show()));
    connect(renderSettings, SIGNAL(aspectRatioChanged(qreal)), ui->widget_2, SLOT(setAspectRatio(qreal)));

    connect(ui->actionProject_Settings, &QAction::triggered, [this] {
        ui->stackedWidget->setCurrentIndex(1);
    });

    connect(ui->page_2->ui->checkBox, SIGNAL(clicked(bool)), ui->widget_2, SLOT(setVisible(bool)));

    connect(renderSettings, &RenderSettings::aspectRatioChanged, ui->camera, &Camera::setAspectRatio);

    auto *ag = new QActionGroup(this);
    ag->addAction(ui->actionSimple);
    ag->addAction(ui->actionCycle);

    connect(ui->page_2->ui->spinBox, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), [this] (int value) {
        timer.setInterval(1000 / value);
    });

    timer.setInterval(1000 / ui->page_2->ui->spinBox->value());

    auto s = new QAction(this), t = new QAction(this);
    s->setSeparator(true);
    t->setSeparator(true);

    ui->menuDockable_dialogs->insertActions(ui->actionError_console, {
                                                ui->mainToolBar->toggleViewAction(),
                                                ui->toolBar->toggleViewAction(),
                                                t,
                                                ui->dockWidget->toggleViewAction(),
                                                ui->dockWidget_2->toggleViewAction(),
                                                ui->dockWidget_3->toggleViewAction(),
                                                s
                                            });

    DockWidget::noneClosedAction()->setDisabled(true);

    ui->menuRecently_closed_docks->insertActions(0, {
                                                     DockWidget::noneClosedAction(),
                                                     ui->dockWidget->recentlyClosedAction(),
                                                     ui->dockWidget_2->recentlyClosedAction(),
                                                     ui->dockWidget_3->recentlyClosedAction()
                                                 });
}

MainWindow::~MainWindow()
{
    delete ui;
}

bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{
    if (Draggable::pressedButton() != Qt::NoButton)
    {
        if (event->type() == QEvent::MouseButtonPress && event->spontaneous())
            return true;

        if (event->type() == QEvent::MouseButtonRelease && reinterpret_cast<QMouseEvent*>(event)->button() != Draggable::pressedButton())
            return true;
    }

    static QObject* tmp = Q_NULLPTR;

    if (event->type() == QEvent::HoverEnter)
    {
        auto tip = watched->property("statusTip");

        if (tip.isValid())
        {
            ui->statusBar->showMessage(tip.toString());
            tmp = watched;
        }
    }

    if (watched == tmp && event->type() == QEvent::HoverLeave)
    {
        ui->statusBar->clearMessage();
        tmp = Q_NULLPTR;
    }

    return QObject::eventFilter(watched, event);
}

#include "../QtChromosomeViz_v2/bartekm_code/PDBSimulation.h"

void MainWindow::openSimulation()
{
    QString path = QFileDialog::getOpenFileName(this, "", "/home", "RCSB Protein Data Bank (*.pdb)");

    if (!path.isEmpty())
    {//TODO tu może być problem z synchronizacją i gubieniem sygnału
        QObject::disconnect(this, SLOT(updateFrameCount(int)));

        simulation = std::make_shared<PDBSimulation>(path.toStdString());

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

        ui->treeView->setModel(simulation->getModel());
    }
}

void MainWindow::updateFrameCount(int n)
{
    bool expandRange = ui->spinBox_3->value() == lastFrame;
    bool expandInterval = ui->horizontalSlider_2->getUpperBound() == lastFrame;

    lastFrame = n - 1;

    ui->spinBox->setMaximum(lastFrame);
    ui->spinBox_2->setMaximum(lastFrame);
    ui->spinBox_3->setMaximum(lastFrame);
    ui->horizontalSlider->setMaximum(lastFrame);
    ui->plot->setMaximum(lastFrame);

    if (expandRange)
        ui->spinBox_3->setValue(lastFrame);

    if (expandInterval)
        ui->horizontalSlider_2->setUpperBound(lastFrame);
}

void MainWindow::setFrame(int n)
{
    currentFrame = n;

    ui->horizontalSlider->setValue(n);
    ui->spinBox->setValue(n);
    ui->scene->setFrame(n);
    ui->plot->setValue(n);
}

void MainWindow::setSoftMinimum(int min)
{
    ui->horizontalSlider->setSoftMinimum(min);
    ui->plot->setSoftMinimum(min);

    softMinimum = min;
}

void MainWindow::setSoftMaximum(int max)
{
    ui->horizontalSlider->setSoftMaximum(max);
    ui->plot->setSoftMaximum(max);

    softMaximum = max;
}

void MainWindow::start()
{
    setFrame(0);
}

void MainWindow::previous()
{
    if (ui->reverse->isChecked())
    {
        if (currentFrame > (ui->actionPreview_range->isChecked() ? softMinimum : 0))
            setFrame(--currentFrame);
        else
        {
            if (ui->actionSimple->isChecked())
                ui->reverse->click();
            else
                setFrame(ui->actionPreview_range->isChecked() ? softMaximum : lastFrame);
        }
    }
    else
    {
        if (currentFrame > 0)
            setFrame(--currentFrame);
    }
}

void MainWindow::reverse(bool checked)
{
    ui->actionPlay_backwards->setChecked(checked);
    ui->reverse->setChecked(checked);

    if (checked)
    {
        if (ui->play->isChecked())
            ui->play->click();

        if (ui->actionPreview_range->isChecked() && (currentFrame <= softMinimum || currentFrame > softMaximum))
            setFrame(softMaximum);

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
    ui->actionPlay_forwards->setChecked(checked);
    ui->play->setChecked(checked);

    if (checked)
    {
        if (ui->reverse->isChecked())
            ui->reverse->click();

        if (ui->actionPreview_range->isChecked() && (currentFrame < softMinimum || currentFrame >= softMaximum))
            setFrame(softMinimum);

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

    if (ui->play->isChecked())
    {
        if (currentFrame < (ui->actionPreview_range->isChecked() ? softMaximum : lastFrame))
            setFrame(++currentFrame);
        else
        {
            if (ui->actionSimple->isChecked())
                ui->play->click();
            else
                setFrame(ui->actionPreview_range->isChecked() ? softMinimum : 0);
        }
    }
    else
    {
        if (currentFrame < lastFrame)
            setFrame(++currentFrame);
    }
}

void MainWindow::end()
{
    setFrame(lastFrame);
    simulation->getFrame(lastFrame+1);//TODO paskudny hack, usunąć po dodaniu wątku
}

void MainWindow::selectAll()
{
    ui->scene->setVisibleSelection(ui->scene->allSelection());
}

void MainWindow::handleSelection(const AtomSelection &selection)
{
    ui->stackedWidget->setCurrentIndex(0);

    if (selection.atomCount())
    {
        ui->camera->setOrigin(selection.weightCenter());
        ui->tabWidget->show();
        z->show();//TODO hack, usunąć
    }
    else
    {
        ui->tabWidget->hide();
        z->hide();//TODO hack, usunąć
    }
}

void MainWindow::setBaseAction(bool enabled)
{
    if (enabled)
    {
        modifiers.last() = qobject_cast<QAction*>(sender());
        connect(ui->camera, SIGNAL(delta(int,int)), ui->camera, mappedSlot[sender()]);
    }
    else
        ui->camera->disconnect(ui->camera);
}

#include "moviemaker.h"

void MainWindow::capture()
{
    MovieMaker::captureScene(ui->scene, ui->camera, renderSettings);
}

#include <QKeyEvent>

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
