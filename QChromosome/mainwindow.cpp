#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ui_projectsettings.h"

#include "../QtChromosomeViz_v2/bartekm_code/PDBSimulationLayer.h"
#include "../QtChromosomeViz_v2/bartekm_code/ProtobufSimulationlayer.h"
#include <QKeyEvent>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    actionGroup(new QActionGroup(this)),
    renderSettings(new RenderSettings())
{
    setCorner(Qt::TopLeftCorner, Qt::LeftDockWidgetArea);
    setCorner(Qt::TopRightCorner, Qt::RightDockWidgetArea);
    setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);
    setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);

    ui->setupUi(this);

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

    connect(ui->actionSettings, SIGNAL(triggered(bool)), renderSettings, SLOT(show()));
    connect(renderSettings, SIGNAL(aspectRatioChanged(qreal)), ui->widget_2, SLOT(setAspectRatio(qreal)));

    connect(ui->actionProject_Settings, &QAction::triggered, [this] {
        ui->dockWidget_2->setWindowTitle("Project settings");
        ui->dockWidget_2->show();

        ui->stackedWidget->setCurrentIndex(0);
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

    addAction(ui->actionViewport);

    connect(ui->actionViewport, &QAction::triggered, [this] {
        ui->dockWidget_2->setWindowTitle("Viewport");
        ui->dockWidget_2->show();

        ui->stackedWidget->setCurrentIndex(3);
    });

    ui->page_4->setVizWidget(ui->scene);
    ui->page_4->setBlind(ui->widget_2);

    newProject();
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
        QString path = QFileDialog::getOpenFileName(this, "", "/home", "Simulation file (*.pdb *.bin)");

        if (!path.isEmpty())
        {
            std::shared_ptr<SimulationLayer> simulationLayer;

            if (path.endsWith(".pdb"))
                simulationLayer = std::make_shared<PDBSimulationLayer>(path.toStdString());
            else
                simulationLayer = std::make_shared<ProtobufSimulationLayer>(path.toStdString());

            simulation->addSimulationLayerConcatenation(std::make_shared<SimulationLayerConcatenation>(simulationLayer));

            ui->scene->setSimulation(simulation);
            ui->plot->setSimulation(simulation);

            connect(ui->treeView->selectionModel(), &QItemSelectionModel::selectionChanged, this, &MainWindow::handleModelSelection);
        }
    } catch (std::exception& e) {
        QMessageBox::critical(this, "Error occured.", e.what());
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
    ui->dockWidget_2->setWindowTitle("Attributes");
    ui->dockWidget_2->show();

    ui->stackedWidget->setCurrentIndex(1);

    ui->camera->setOrigin(selection.atomCount() ? selection.weightCenter() : QVector3D(0, 0, 0));

    ui->page_2->handleSelection(selection);
}

void dumpModel(const QAbstractItemModel* model, const QModelIndex& root, QList<unsigned int>& id)
{
    auto v = root.sibling(root.row(), 2).data();

    if (v.canConvert<uint>())
        id.append(v.toUInt() - 1);

    for (int r = 0; r < model->rowCount(root); r++)
        dumpModel(model, root.child(r, 0), id);
}

void MainWindow::handleModelSelection()
{
    QList<unsigned int> id;
    QSet<int> type;

    for (auto r : ui->treeView->selectionModel()->selectedRows())
    {
        auto v = r.sibling(r.row(), 1).data();

        if (v.canConvert<int>())
            type.insert(v.toInt());

        dumpModel(ui->treeView->model(), r, id);
    }

    auto selection = ui->scene->customSelection(id);

    ui->scene->setVisibleSelection(selection, false);

    if (type.size() == 1 && type.toList().first() == ObjectType::LayerObject)
    {
        ui->dockWidget_2->setWindowTitle("Attributes");
        ui->dockWidget_2->show();

        ui->stackedWidget->setCurrentIndex(2);
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
        ui->camera->disconnect(ui->camera);
}

#include "moviemaker.h"

void MainWindow::capture()
{
    QString suffix = renderSettings->timestamp() ? QDateTime::currentDateTime().toString(Qt::ISODate) : "";
    MovieMaker::captureScene(ui->scene, ui->camera, renderSettings, suffix);

    system(QString(QString("rm ") + renderSettings->saveFile() + suffix + ".pov").toUtf8().constData());
    system("rm povray.ini");
}

void MainWindow::captureMovie()
{
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

    MovieMaker::makeMovie(renderSettings->saveFile(), frames, ui->page->ui->spinBox->value(), ui->page->ui->spinBox->value(), renderSettings->timestamp());

    system(QString(QString("find . -regextype sed -regex \".*/") + renderSettings->saveFile() + "[0-9]\\{"
                   + QString::number(QString::number(frames).length()) + "\\}\\.\\(png\\|pov\\)\" -delete").toUtf8().constData());
    system("rm povray.ini");
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
