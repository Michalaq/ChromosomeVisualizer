#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ui_projectsettings.h"
#include "ui_camerasettings.h"

#include "../QtChromosomeViz_v2/bartekm_code/PDBSimulationLayer.h"
#include "../QtChromosomeViz_v2/bartekm_code/ProtobufSimulationlayer.h"
#include <QKeyEvent>
#include "visibilitydelegate.h"
#include "namedelegate.h"
#include "tagsdelegate.h"
#include <QtConcurrent/QtConcurrentRun>

static const char * ext = ".qcs";

Interpolator ip;
int ignore;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    actionGroup(new QActionGroup(this)),
    renderSettings(RenderSettings::getInstance()),
    preferences(new Preferences),
    materialBrowser(MaterialBrowser::getInstance())
{
    setWindowTitle("QChromosome 4D Studio - [Untitled]");

    setCorner(Qt::TopLeftCorner, Qt::LeftDockWidgetArea);
    setCorner(Qt::TopRightCorner, Qt::RightDockWidgetArea);
    setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);
    setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);

    ui->setupUi(this);

    connect(ui->actionInfo, &QAction::triggered, [this] {
        QMessageBox::about(0, "About QChromosome 4D", "Lorem ipsum dolor sit amet, consectetur adipiscing elit. In hendrerit arcu eu bibendum laoreet. Pellentesque habitant morbi tristique senectus et netus et malesuada fames ac turpis egestas. Sed ultricies consectetur nunc, in mollis libero malesuada vel. In nec ultrices dolor. Aenean nulla nisl, condimentum viverra molestie et, lobortis efficitur metus. Suspendisse eget condimentum mi, eget placerat nisl. Phasellus sit amet enim nulla. Ut vel enim ac lacus convallis sagittis. Vivamus dapibus felis magna, non dictum dolor finibus non. Cras porta nec risus ac tincidunt. Aliquam nisi arcu, dapibus ut nisl vel, pretium convallis nunc. Praesent ac rhoncus metus. Vivamus est nunc, finibus et dolor a, cursus sollicitudin lectus.");
    });

    actionGroup->addAction(ui->actionSelect);
    actionGroup->addAction(ui->actionCustom);
    actionGroup->addAction(ui->actionMove);
    actionGroup->addAction(ui->actionRotate);
    actionGroup->addAction(ui->actionScale);

    for (auto action : actionGroup->actions())
        connect(action, &QAction::toggled, [action, this] (bool checked) {
            if (checked)
            {
                auto text = action->property("cameraStatusTip").toString();
                ui->canvas->setStatusTip(text);
                ui->statusBar->showMessage(text);
            }
        });

    bindings.insert(Qt::Key_E, ui->actionMove);
    bindings.insert(Qt::Key_R, ui->actionRotate);
    bindings.insert(Qt::Key_T, ui->actionScale);

    modifiers.push_back(ui->actionMove);

    connect(ui->spinBox_2, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), [this](int val) {
        ui->horizontalSlider_2->setMinimum(val);
        ui->spinBox_3->setMinimum(val);
        ui->page->ui->spinBox_6->setMinimum(val);
        ui->page->ui->spinBox_4->setMinimum(val);
        ui->page->ui->spinBox_3->setValue(val, false);
    });

    connect(ui->spinBox_3, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), [this](int val) {
        ui->horizontalSlider_2->setMaximum(val);
        ui->spinBox_2->setMaximum(val);
        ui->page->ui->spinBox_3->setMaximum(val);
        ui->page->ui->spinBox_7->setMaximum(val);
        ui->page->ui->spinBox_6->setValue(val, false);
    });

    /* connect actions */
    mappedSlot[ui->actionMove] = Camera::CA_Move;
    mappedSlot[ui->actionRotate] = Camera::CA_Rotate;
    mappedSlot[ui->actionScale] = Camera::CA_Scale;

    ui->actionMove->toggle();

    connect(ui->actionSettings, SIGNAL(triggered(bool)), renderSettings, SLOT(show()));
    connect(renderSettings, SIGNAL(aspectRatioChanged(qreal)), ui->widget_2, SLOT(setAspectRatio(qreal)));

    connect(ui->actionPreferences, SIGNAL(triggered(bool)), preferences, SLOT(show()));

    connect(ui->actionProject_Settings, &QAction::triggered, [this] {
        ui->stackedWidget->setCurrentIndex(0);
        ui->dockWidget_2->show();
    });

    connect(renderSettings, &RenderSettings::aspectRatioChanged, ui->camera, &Camera::setAspectRatio);

    connect(ui->actionMaterial_Browser, SIGNAL(triggered(bool)), materialBrowser, SLOT(show()));
    connect(ui->actionContent_Browser, SIGNAL(triggered(bool)), materialBrowser, SLOT(show()));

    auto *ag = new QActionGroup(this);
    ag->addAction(ui->actionSimple);
    ag->addAction(ui->actionCycle);

    connect(ui->page->ui->spinBox, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), [this] (int value) {
        timer.setInterval(1000 / value);
    });

    timer.setInterval(1000 / ui->page->ui->spinBox->value());

    connect(ui->page->ui->spinBox_3, SIGNAL(valueChanged(int)), ui->spinBox_2, SLOT(setValue(int)));
    connect(ui->page->ui->spinBox_4, SIGNAL(valueChanged(int)), this, SLOT(setSoftMinimum(int)));
    connect(ui->page->ui->spinBox_5, SIGNAL(valueChanged(int)), this, SLOT(setFrame(int)));
    connect(ui->page->ui->spinBox_6, SIGNAL(valueChanged(int)), ui->spinBox_3, SLOT(setValue(int)));
    connect(ui->page->ui->spinBox_7, SIGNAL(valueChanged(int)), this, SLOT(setSoftMaximum(int)));

    auto s = new QAction(this), t = new QAction(this);
    s->setSeparator(true);
    t->setSeparator(true);

    ui->menuDockable_dialogs->insertActions(ui->actionError_console, {
                                                ui->mainToolBar->toggleViewAction(),

                                                t,
                                                ui->dockWidget->toggleViewAction(),
                                                ui->dockWidget_2->toggleViewAction(),
                                                ui->dockWidget_3->toggleViewAction(),
                                                s
                                            });

    ui->menuRecently_closed_docks->insertActions(0, {
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

    //TODO reimplement this to work with many cameras
    connect(ui->record, &MediaControl::toggled, [this](bool checked) {
        ip.setRecordingState(checked);
        if (checked)
        {
            ui->canvas->setStyleSheet("background: #d40000;");
            connect(ui->camera, &Camera::modelViewChanged, &ip, &Interpolator::recordKeyframe);
            connect(ui->camera, &Camera::projectionChanged, &ip, &Interpolator::recordKeyframe);
        }
        else
        {
            ui->canvas->setStyleSheet("background: #4d4d4d;");
            disconnect(ui->camera, &Camera::modelViewChanged, &ip, &Interpolator::recordKeyframe);
            disconnect(ui->camera, &Camera::projectionChanged, &ip, &Interpolator::recordKeyframe);
        }
    });

    connect(ui->key, &MediaControl::clicked, [this] {
        qobject_cast<Camera*>(ui->stackedWidget_2->currentWidget())->captureFrame();
        ui->horizontalSlider->update();
    });

    ip.setKey(ui->spinBox);

    auto p = ui->page_5->ui;
    ip.trackValues({ p->doubleSpinBox, p->doubleSpinBox_2, p->doubleSpinBox_3, p->doubleSpinBox_7, p->doubleSpinBox_8, p->doubleSpinBox_9, p->doubleSpinBox_10, p->doubleSpinBox_11, p->doubleSpinBox_12 });

    ui->horizontalSlider->setSplineInterpolator(ui->camera);

    connect(ui->camera, &SplineInterpolator::selectionChanged, [this] {
        ui->page_6->setSplineInterpolator(ui->camera);
        ui->stackedWidget->setCurrentIndex(5);
        ui->dockWidget_2->show();
    });

    connect(ui->actionCoordinates, &QAction::toggled, [this](bool c) {
        qobject_cast<Camera*>(ui->stackedWidget_2->currentWidget())->setRotationType(c ? Camera::RT_Camera : Camera::RT_World);
    });

    connect(ui->camera, &Camera::rotationTypeChanged, [this](int i) {
        ui->actionCoordinates->setChecked(i == 1);
    });

    connect(ui->actionFocus, &QAction::triggered, [this] {
        focusSelection(ui->scene->selectedSpheresObject());
    });

    connect(ui->actionSelect, &QAction::toggled, [this](bool checked) {
        if (checked)
            ui->widget_3->setSelectionType(SelectionType::RECTANGULAR_SELECTION);
    });

    connect(ui->actionCustom, &QAction::toggled, [this](bool checked) {
        if (checked)
            ui->widget_3->setSelectionType(SelectionType::CUSTOM_SHAPE_SELECTION);
    });

    connect(ui->actionSnapshot, &QAction::triggered, [this] {
        QtConcurrent::run(this, &MainWindow::capture);
    });

    connect(ui->actionFilm, &QAction::triggered, [this] {
        QtConcurrent::run(this, &MainWindow::captureMovie);
    });

    ui->treeView->setItemDelegateForColumn(0, new NameDelegate(ui->page_2));
    ui->treeView->setItemDelegateForColumn(3, new VisibilityDelegate(this));
    ui->treeView->setItemDelegateForColumn(5, new TagsDelegate(this));

    ui->plot->followSlider(ui->horizontalSlider);

    connect(ui->treeView, SIGNAL(visibilityChanged(VisibilityMode)), ui->page_2, SLOT(updateVisibility(VisibilityMode)));

    connect(materialBrowser, &MaterialBrowser::materialsSelected, [this](const QList<Material*>& selected) {
        ui->page_7->handleSelection(selected);
        ui->stackedWidget->setCurrentIndex(6);
    });

    connect(ui->page_7, SIGNAL(attributesChanged(const Material*)), ui->treeView, SLOT(updateAttributes(const Material*)));
    connect(ui->page_7, SIGNAL(attributesChanged(const Material*)), materialBrowser, SLOT(update()));

    connect(ui->actionCamera, &QAction::triggered, [this] {
        addCamera(new Camera(*qobject_cast<Camera*>(ui->stackedWidget_2->currentWidget())));
    });

    connect(ui->treeView, &TreeView::cameraChanged, [this](Camera* camera) {
        if (!camera) camera = ui->camera;
        ui->stackedWidget_2->currentWidget()->blockSignals(true);
        camera->blockSignals(false);
        ui->stackedWidget_2->setCurrentWidget(camera);
        ui->horizontalSlider->setSplineInterpolator(camera);
    });

    newProject();

    ui->treeView->header()->resizeSection(3, 40);
    ui->treeView->header()->setSectionResizeMode(3, QHeaderView::Fixed);
    ui->treeView->header()->setSectionResizeMode(5, QHeaderView::Fixed);
    ui->treeView->setScene(ui->scene);
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

    if (watched == ui->widget_3 && event->type() == QEvent::Wheel)
    {
        QApplication::sendEvent(ui->stackedWidget_2->currentWidget(), event);
        return true;
    }

    return QObject::eventFilter(watched, event);
}

#include <QJsonObject>

void MainWindow::read(const QJsonObject &json)
{
    const QJsonObject children = json["Descendants"].toObject();

    for (auto child = children.end() - 1; child != children.begin() - 1; child--)
    {
        const QJsonObject object = child.value().toObject()["Object"].toObject();

        if (object["class"] == "Layer")
        {
            auto simulationLayer = std::make_shared<SimulationLayerConcatenation>();
            simulationLayer->read(object["paths"].toArray());

            simulation->addSimulationLayerConcatenation(simulationLayer, false);
        }

        if (object["class"] == "Camera")
        {
            auto camera = new Camera();
            camera->read(object);

            addCamera(camera);
        }
    }
}

void MainWindow::newProject()
{
    currentFile.clear();
    ui->page->ui->lineEdit_6->setText(QDir(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)).filePath("Untitled"));
    setWindowTitle("QChromosome 4D Studio - [Untitled]");

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
    ui->treeView->hideColumn(4);
    ui->treeView->hideColumn(6);
    ui->treeView->setColumnWidth(3, 48);

    connect(ui->treeView->selectionModel(), &QItemSelectionModel::selectionChanged, this, &MainWindow::handleModelSelection);

    ui->stackedWidget->setCurrentIndex(0);
}

#include <QStandardPaths>

void MainWindow::openProject()
{
    QString path = QFileDialog::getOpenFileName(0, "", QStandardPaths::writableLocation(QStandardPaths::HomeLocation), QString("QChromosome 4D Project File (*%1)").arg(ext));

    if (!path.isEmpty())
    {
        newProject();

        currentFile = path;
        ui->page->ui->lineEdit_6->setText(path);
        setWindowTitle(QString("QChromosome 4D Studio - [%1]").arg(QFileInfo(path).fileName()));

        QFile file(path);
        file.open(QIODevice::ReadOnly | QIODevice::Text);
        const QJsonObject project = QJsonDocument::fromJson(file.readAll()).object();
        file.close();

        const QJsonObject viewport = project["Viewport"].toObject();
        ui->page_4->read(viewport);

        const QJsonObject camera = project["Camera"].toObject();
        ui->camera->read(camera);

        const QJsonObject objects = project["Objects"].toObject();
        read(objects);
        simulation->getModel()->read(objects);

        ui->scene->setSimulation(simulation);
        ui->plot->updateSimulation();

        const QJsonArray materials = project["Materials"].toArray();
        materialBrowser->read(materials);

        ui->treeView->read(objects);

        const QJsonObject bar = project["bar"].toObject();
        ui->scene->read(bar);

        const QJsonObject projectSettings = project["Project Settings"].toObject();
        ui->page->read(projectSettings);
    }
}

void MainWindow::addLayer()
{
    try {
        QString path = QFileDialog::getOpenFileName(0, "", QSettings().value("locallib").toString(), "All QChromosome 4D Files (*.pdb *.bin);;RCSB Protein Data Bank (*.pdb);;Motions (*.bin)");

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

            ui->treeView->materializeTags();
        }
    } catch (std::exception& e) {
        QMessageBox::critical(0, "Error occured.", e.what());
    }
}

void MainWindow::saveProject()
{
    if (currentFile.isEmpty())
        saveProjectAs();
    else
    {
        QJsonObject project;

        ui->page->ui->lineEdit_4->setText(QString("QChromosome 4D File (*%1)").arg(ext));
        ui->page->ui->lineEdit_5->setText("1.01");

        QJsonObject projectSettings;
        ui->page->write(projectSettings);
        project["Project Settings"] = projectSettings;

        QJsonObject viewport;
        ui->page_4->write(viewport);
        project["Viewport"] = viewport;

        QJsonObject camera;
        ui->camera->write(camera);
        project["Camera"] = camera;

        QJsonObject objects;
        simulation->getModel()->write(objects);
        project["Objects"] = objects;

        QJsonArray materials;
        materialBrowser->write(materials);
        project["Materials"] = materials;

        QJsonObject bar;
        ui->scene->write(bar);
        project["bar"] = bar;

        QFile file(currentFile);
        file.open(QIODevice::WriteOnly | QIODevice::Text);
        file.write(QJsonDocument(project).toJson());
        file.close();
    }
}

void MainWindow::saveProjectAs()
{
    QString path = QFileDialog::getSaveFileName(0, "", currentFile.isEmpty() ? QStandardPaths::writableLocation(QStandardPaths::HomeLocation) : currentFile, QString("QChromosome 4D Project File (*%1)").arg(ext));

    if (!path.isEmpty())
    {
        QFileInfo info(path);
        path = info.dir().filePath(info.completeBaseName().append(ext));

        currentFile = path;
        ui->page->ui->lineEdit_6->setText(path);
        setWindowTitle(QString("QChromosome 4D Studio - [%1]").arg(info.fileName()));

        saveProject();
    }
}

void MainWindow::updateFrameCount(int n)
{
    bool expandRange = ui->spinBox_3->value() == lastFrame;
    bool expandInterval = ui->horizontalSlider_2->getUpperBound() == lastFrame;

    lastFrame = n - 1;

    ui->spinBox->setMaximum(lastFrame);
    ui->spinBox_3->setMaximum(lastFrame);
    ui->horizontalSlider->setMaximum(lastFrame);
    ui->plot->setMaximum(lastFrame);
    ui->page->ui->spinBox_5->setMaximum(lastFrame);
    ui->page->ui->spinBox_6->setMaximum(lastFrame);

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
    SplineInterpolator::setFrame(n);
    ui->page->ui->spinBox_5->setValue(n, false);
}

void MainWindow::setSoftMinimum(int min)
{
    ui->horizontalSlider->setSoftMinimum(min);
    ui->horizontalSlider_2->setLowerBound(min, false);
    ui->plot->setSoftMinimum(min);
    ui->page->ui->spinBox_7->setMinimum(min);
    ui->page->ui->spinBox_4->setValue(min, false);

    softMinimum = min;
}

void MainWindow::setSoftMaximum(int max)
{
    ui->horizontalSlider->setSoftMaximum(max);
    ui->horizontalSlider_2->setUpperBound(max, false);
    ui->plot->setSoftMaximum(max);
    ui->page->ui->spinBox_4->setMaximum(max);
    ui->page->ui->spinBox_7->setValue(max, false);

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

void MainWindow::handleSelection(const AtomSelection &selection, bool b)
{
    ui->dockWidget_2->show();

    ui->stackedWidget->setCurrentIndex(1);

    ui->page_2->handleSelection(selection);

    Camera::setOrigin(selection.weightCenter());

    if (b) ui->treeView->setSelection(selection.selectedIndices());
}

void dumpModel(const QAbstractItemModel* model, const QModelIndex& root, QVector<QList<unsigned int>>& id)
{
    auto t = root.sibling(root.row(), 1).data().toInt();
    auto v = root.sibling(root.row(), 2).data();

    if (v.canConvert<uint>())
        id[t].append(v.toUInt());

    for (int r = 0; r < model->rowCount(root); r++)
        dumpModel(model, model->index(r, 0, root), id);
}

void MainWindow::handleModelSelection()
{
    QVector<QList<unsigned int>> id(6);
    QSet<int> type;

    for (auto r : ui->treeView->selectionModel()->selectedRows())
    {
        auto t = r.sibling(r.row(), 1).data().toInt();

        type.insert(t);

        dumpModel(ui->treeView->model(), r, id);
    }

    auto selection = ui->scene->customSelection(id[NodeType::AtomObject]);

    ui->scene->setVisibleSelection(selection, false);
    handleSelection(selection, false);

    // handle custom selection types
    if (type.size() == 1)
    {
        if (*type.begin() == NodeType::CameraObject)
        {
            ui->page_5->handleSelection(ui->treeView->selectionModel()->selectedRows());
            ui->stackedWidget->setCurrentIndex(4);
            ui->dockWidget_2->show();
        }

        /*if (*type.begin() == NodeType::LayerObject)
        {
            ui->dockWidget_2->show();

            ui->stackedWidget->setCurrentIndex(2);

            ui->page_3->handleSelection(selection, id[NodeType::LayerObject]);
        }*/
    }
}

void MainWindow::focusSelection(const AtomSelection& s)
{
    qobject_cast<Camera*>(ui->stackedWidget_2->currentWidget())->setLookAt(s.weightCenter());
}

void MainWindow::setBaseAction(bool enabled)
{
    if (enabled)
    {
        modifiers.last() = qobject_cast<QAction*>(sender());
        Camera::setCurrentAction(mappedSlot[sender()]);
    }
}

#include "moviemaker.h"

void MainWindow::capture() const
{
    QString suffix = renderSettings->timestamp() ? QDateTime::currentDateTime().toString("yyyy'-'MM'-'dd'T'HH'-'mm'-'ss") : "";
    MovieMaker::captureScene1(currentFrame, ui->scene, qobject_cast<Camera*>(ui->stackedWidget_2->currentWidget()), suffix);

    if (renderSettings->render() && renderSettings->openFile())
        QProcess::execute("xdg-open", {renderSettings->saveFile() + suffix + ".png"});
}

void MainWindow::captureMovie() const
{
    QString suffix = renderSettings->timestamp() ? QDateTime::currentDateTime().toString("yyyy'-'MM'-'dd'T'HH'-'mm'-'ss") : "";
    MovieMaker::captureScene(ui->horizontalSlider_2->getLowerBound(), ui->horizontalSlider_2->getUpperBound(), ui->scene, qobject_cast<Camera*>(ui->stackedWidget_2->currentWidget()), ip, suffix, ui->page->ui->spinBox->value());

    if (renderSettings->render() && renderSettings->openFile())
        QProcess::execute("xdg-open", {renderSettings->saveFile() + suffix + ".mp4"});
}

void MainWindow::updateLocks()
{
    Camera::lockCoordinates(!ui->actionXLock->isChecked(), !ui->actionYLock->isChecked(), !ui->actionZLock->isChecked());
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

void MainWindow::closeEvent(QCloseEvent *event)
{
    switch (QMessageBox::question(0, "QChromosome 4D Studio", QString("Do you want to save the changes to the project \"%1\" before quitting?").arg(currentFile.isEmpty() ? "Untitled" : QFileInfo(currentFile).fileName()), QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel, QMessageBox::Yes))
    {
    case QMessageBox::Yes:
        saveProject();
    case QMessageBox::No:
        event->accept();
        QApplication::quit();;
    case QMessageBox::Cancel:
        event->ignore();
        break;
    }
}

void MainWindow::addCamera(Camera* camera)
{
    camera->blockSignals(true);

    connect(camera, &Camera::modelViewChanged, ui->scene, &VizWidget::setModelView);
    connect(camera, &Camera::projectionChanged, ui->scene, &VizWidget::setProjection);
    connect(camera, &Camera::modelViewChanged, ui->widget, &Axis::setModelView);
    connect(renderSettings, &RenderSettings::aspectRatioChanged, camera, &Camera::setAspectRatio);
    connect(camera, &Camera::rotationTypeChanged, [this](int i) {
        ui->actionCoordinates->setChecked(i == 1);
    });
    connect(camera, &SplineInterpolator::selectionChanged, [=] {
        ui->page_6->setSplineInterpolator(camera);
        ui->stackedWidget->setCurrentIndex(5);
        ui->dockWidget_2->show();
    });

    ui->stackedWidget_2->addWidget(camera);

    ((TreeModel*)ui->treeView->model())->addCamera(camera);
}
