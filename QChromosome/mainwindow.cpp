#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ui_projectsettings.h"
#include "ui_camerasettings.h"

#include "../QtChromosomeViz_v2/bartekm_code/PDBSimulationLayer.h"
#include "../QtChromosomeViz_v2/bartekm_code/ProtobufSimulationlayer.h"
#include <QKeyEvent>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    actionGroup(new QActionGroup(this)),
    renderSettings(new RenderSettings()),
    rsw(new RenderSettingsWidget(renderSettings)),
    ignore(0)
{
    setCorner(Qt::TopLeftCorner, Qt::LeftDockWidgetArea);
    setCorner(Qt::TopRightCorner, Qt::RightDockWidgetArea);
    setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);
    setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);

    ui->setupUi(this);

    connect(ui->actionInfo, &QAction::triggered, [this] {
        QMessageBox::about(0, "About QChromosome 4D", "Lorem ipsum dolor sit amet, consectetur adipiscing elit. In hendrerit arcu eu bibendum laoreet. Pellentesque habitant morbi tristique senectus et netus et malesuada fames ac turpis egestas. Sed ultricies consectetur nunc, in mollis libero malesuada vel. In nec ultrices dolor. Aenean nulla nisl, condimentum viverra molestie et, lobortis efficitur metus. Suspendisse eget condimentum mi, eget placerat nisl. Phasellus sit amet enim nulla. Ut vel enim ac lacus convallis sagittis. Vivamus dapibus felis magna, non dictum dolor finibus non. Cras porta nec risus ac tincidunt. Aliquam nisi arcu, dapibus ut nisl vel, pretium convallis nunc. Praesent ac rhoncus metus. Vivamus est nunc, finibus et dolor a, cursus sollicitudin lectus.");
    });

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

    connect(ui->spinBox_2, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), ui->horizontalSlider_2, &RangeSlider::setMinimum);
    connect(ui->spinBox_2, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), ui->spinBox_3, &SpinBox::setMinimum);
    connect(ui->spinBox_3, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), ui->horizontalSlider_2, &RangeSlider::setMaximum);
    connect(ui->spinBox_3, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), ui->spinBox_2, &SpinBox::setMaximum);

    /* connect actions */
    mappedSlot[ui->actionMove] = SLOT(move(int,int));
    mappedSlot[ui->actionRotate] = SLOT(rotate(int,int));
    mappedSlot[ui->actionScale] = SLOT(scale(int,int));

    ui->actionMove->toggle();

    connect(ui->actionSettings, SIGNAL(triggered(bool)), rsw, SLOT(show()));
    connect(renderSettings, SIGNAL(aspectRatioChanged(qreal)), ui->widget_2, SLOT(setAspectRatio(qreal)));

    connect(ui->actionProject_Settings, &QAction::triggered, [this] {
        ui->stackedWidget->setCurrentIndex(0);
        ui->dockWidget_2->show();
    });

    connect(renderSettings, &RenderSettings::aspectRatioChanged, ui->camera, &Camera::setAspectRatio);

    auto *ag = new QActionGroup(this);
    ag->addAction(ui->actionSimple);
    ag->addAction(ui->actionCycle);

    connect(ui->page->ui->spinBox, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), [this] (int value) {
        timer.setInterval(1000 / value);
    });

    timer.setInterval(1000 / ui->page->ui->spinBox->value());

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

    ui->page_2->setVizWidget(ui->scene);

    ui->page_3->setVizWidget(ui->scene);

    addAction(ui->actionViewport);

    connect(ui->actionViewport, &QAction::triggered, [this] {
        ui->stackedWidget->setCurrentIndex(3);
        ui->dockWidget_2->show();
    });

    ui->page_4->setVizWidget(ui->scene);
    ui->page_4->setBlind(ui->widget_2);
    ui->page_4->setAxis(ui->widget);

    connect(ui->actionCamera, &QAction::triggered, [this] {
        ui->stackedWidget->setCurrentIndex(4);
        ui->dockWidget_2->show();
    });

    ui->page_5->setCamera(ui->camera);

    connect(ui->record, &MediaControl::toggled, [this](bool checked) {
        ip.setRecordingState(checked);
        if (checked)
        {
            ui->canvas->setStyleSheet("background: #d40000;");
            connect(ui->camera, &Camera::modelViewChanged, this, &MainWindow::recordKeyframe);
        }
        else
        {
            ui->canvas->setStyleSheet("background: #4d4d4d;");
            disconnect(ui->camera, &Camera::modelViewChanged, this, &MainWindow::recordKeyframe);
        }
    });

    connect(ui->key, &MediaControl::clicked, [this] {
        ip.recordKeyframe(currentFrame, {ui->camera->position(), ui->camera->EulerAngles()});
        ui->horizontalSlider->update();
    });

    ip.trackKeys({ui->page_5->ui->doubleSpinBox_7, ui->page_5->ui->doubleSpinBox_8, ui->page_5->ui->doubleSpinBox_9, ui->page_5->ui->doubleSpinBox_10, ui->page_5->ui->doubleSpinBox_11, ui->page_5->ui->doubleSpinBox_12});

    ui->horizontalSlider->setInterpolator(&ip);

    ui->page_6->setInterpolator(&ip);

    connect(ui->horizontalSlider, &Slider::keyframeSelected, [this](int frame) {//podłączyć do interpolatora
        if (frame >= 0)
        {
            ui->page_6->updateContents();

            ui->stackedWidget->setCurrentIndex(5);
            ui->dockWidget_2->show();
            ui->page_6->show();
        }
        else
            ui->page_6->hide();
    });

    connect(&ip, &Interpolator::interpolationChanged, [this] {
        setFrame(currentFrame);
        ui->horizontalSlider->update();
    });

    connect(ui->actionCoordinates, &QAction::toggled, [this](bool c) {
        ui->page_5->setRotationType(c ? Camera::RT_Camera : Camera::RT_World);
    });

    newProject();
}

void MainWindow::recordKeyframe()
{
    ip.recordKeyframe(currentFrame, {});
}

MainWindow::~MainWindow()
{
    delete ui;
    delete rsw;
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

void MainWindow::newProject()
{
    simulation = std::make_shared<Simulation>();

    connect(simulation.get(), SIGNAL(frameCountChanged(int)), this, SLOT(updateFrameCount(int)));

    ui->plot->setRange(0, 0);
    ui->horizontalSlider->setRange(0, 0);
    ui->horizontalSlider_2->setRange(0, 0);

    currentFrame = 0;
    lastFrame = 0;

    softMinimum = 0;
    softMaximum = 0;

    ui->scene->setSimulation(simulation);
    ui->plot->setSimulation(simulation);
    ui->page_3->setSimulation(simulation);

    ui->treeView->setModel(simulation->getModel());
    ui->treeView->hideColumn(1);
    ui->treeView->hideColumn(2);
}

void MainWindow::openProject()
{
    addLayer();//TODO tymczasowo
}

void MainWindow::addLayer()
{
    try {
        QString path = QFileDialog::getOpenFileName(0, "", "/home", "Simulation file (*.pdb *.bin)");

        if (!path.isEmpty())
        {
            std::shared_ptr<SimulationLayer> simulationLayer;

            if (path.endsWith(".pdb"))
                simulationLayer = std::make_shared<PDBSimulationLayer>(path.toStdString());
            else
                simulationLayer = std::make_shared<ProtobufSimulationLayer>(path.toStdString());

            simulation->addSimulationLayerConcatenation(std::make_shared<SimulationLayerConcatenation>(simulationLayer));

            ui->scene->setSimulation(simulation);
            ui->plot->updateSimulation();

            connect(ui->treeView->selectionModel(), &QItemSelectionModel::selectionChanged, this, &MainWindow::handleModelSelection);
        }
    } catch (std::exception& e) {
        QMessageBox::critical(0, "Error occured.", e.what());
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
    ip.setFrame(n);
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
    frameNumber_t previousFrame = simulation->getPreviousTime(currentFrame);

    if (currentFrame > 0)
    {
        currentFrame = previousFrame;
        setFrame(currentFrame);
    }
}

void MainWindow::reverse_previous()
{
    qint64 previousFrame = qMax(currentFrame - qRound(1. * time.restart() * ui->page->ui->spinBox->value() / 1000), 0);

    if (currentFrame > (ui->actionPreview_range->isChecked() ? softMinimum : 0))
    {
        currentFrame = previousFrame;
        setFrame(currentFrame);
    }
    else
    {
        if (ui->actionSimple->isChecked())
            ui->reverse->click();
        else
        {
            currentFrame = ui->actionPreview_range->isChecked() ? softMaximum : lastFrame;
            setFrame(currentFrame);
        }
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

        connect(&timer, SIGNAL(timeout()), this, SLOT(reverse_previous()));

        time.restart();
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

        connect(&timer, SIGNAL(timeout()), this, SLOT(play_next()));

        time.restart();
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
    frameNumber_t nextFrame = simulation->getNextTime(currentFrame);
    simulation->getFrame(nextFrame);

    if (currentFrame < lastFrame)
    {
        currentFrame = nextFrame;
        setFrame(currentFrame);
    }
}

void MainWindow::play_next()
{
    qint64 nextFrame = currentFrame + qRound(1. * time.restart() * ui->page->ui->spinBox->value() / 1000);

    if (currentFrame < (ui->actionPreview_range->isChecked() ? softMaximum : lastFrame))
    {
        currentFrame = nextFrame;
        setFrame(currentFrame);
    }
    else
    {
        if (ui->actionSimple->isChecked())
            ui->play->click();
        else
        {
            currentFrame = ui->actionPreview_range->isChecked() ? softMinimum : 0;
            setFrame(currentFrame);
        }
    }

    simulation->getFrame(currentFrame+1);//TODO paskudny hack, usunąć po dodaniu wątku
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
    ui->dockWidget_2->show();

    ui->stackedWidget->setCurrentIndex(1);

    ui->page_2->handleSelection(selection);

    ui->camera->setOrigin(selection.atomCount() ? selection.weightCenter() : QVector3D(0, 0, 0));
}

void dumpModel(const QAbstractItemModel* model, const QModelIndex& root, QVector<QList<unsigned int>>& id)
{
    auto t = root.sibling(root.row(), 1).data().toInt();
    auto v = root.sibling(root.row(), 2).data();

    if (v.canConvert<uint>())
        id[t].append(v.toUInt());

    for (int r = 0; r < model->rowCount(root); r++)
        dumpModel(model, root.child(r, 0), id);
}

void MainWindow::handleModelSelection()
{
    QVector<QList<unsigned int>> id(5);
    QSet<int> type;

    for (auto r : ui->treeView->selectionModel()->selectedRows())
    {
        auto t = r.sibling(r.row(), 1).data().toInt();

        type.insert(t);

        dumpModel(ui->treeView->model(), r, id);
    }

    auto selection = ui->scene->customSelection(id[NodeType::AtomObject]);

    ui->scene->setVisibleSelection(selection, false);

    if (type.size() == 1 && *type.begin() == NodeType::LayerObject)
    {
        ui->dockWidget_2->show();

        ui->stackedWidget->setCurrentIndex(2);

        ui->page_3->handleSelection(selection, id[NodeType::LayerObject]);
    }
    else
        handleSelection(selection);
}

void MainWindow::setBaseAction(bool enabled)
{
    if (enabled)
    {
        modifiers.last() = qobject_cast<QAction*>(sender());
        connect(ui->camera, SIGNAL(delta(int,int)), ui->camera, mappedSlot[sender()]);
    }
    else
        disconnect(ui->camera, SIGNAL(delta(int,int)), ui->camera, mappedSlot[sender()]);
}

#include "moviemaker.h"

void MainWindow::capture()
{
    QString suffix = renderSettings->timestamp() ? QDateTime::currentDateTime().toString("yyyy'-'MM'-'dd'T'HH'-'mm'-'ss") : "";
    MovieMaker::captureScene(ui->scene, ui->camera, renderSettings, suffix);

    system(QString(QString("rm ") + renderSettings->saveFile() + suffix + ".pov").toUtf8().constData());
    system("rm povray.ini");

    if (renderSettings->openFile())
        system(QString(QString("xdg-open ") + renderSettings->saveFile() + suffix + ".png").toUtf8().constData());
}

void MainWindow::captureMovie()
{
    QString suffix = renderSettings->timestamp() ? QDateTime::currentDateTime().toString("yyyy'-'MM'-'dd'T'HH'-'mm'-'ss") : "";

    ui->scene->setFrame(ui->horizontalSlider_2->getLowerBound());
    int frames = ui->horizontalSlider_2->getUpperBound() - ui->horizontalSlider_2->getLowerBound() + 1;
    for (int i = 1; ; i++)
    {
        MovieMaker::captureScene(ui->scene, ui->camera, renderSettings, QString::number(i).rightJustified(QString::number(frames).length(), '0'));
        if (i != frames)
            ui->scene->advanceFrame();
        else
            break;
    }

    MovieMaker::makeMovie(renderSettings->saveFile(), frames, ui->page->ui->spinBox->value(), ui->page->ui->spinBox->value(), suffix);

    system(QString(QString("find . -regextype sed -regex \".*/") + renderSettings->saveFile() + "[0-9]\\{"
                   + QString::number(QString::number(frames).length()) + "\\}\\.\\(png\\|pov\\)\" -delete").toUtf8().constData());
    system("rm povray.ini");

    if (renderSettings->openFile())
        system(QString(QString("xdg-open ") + renderSettings->saveFile() + suffix + ".mp4").toUtf8().constData());
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
