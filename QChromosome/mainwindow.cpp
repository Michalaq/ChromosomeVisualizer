#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ui_projectsettings.h"

#include "../QtChromosomeViz_v2/bartekm_code/PDBSimulationLayer.h"
#include "../QtChromosomeViz_v2/bartekm_code/ProtobufSimulationlayer.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    actionGroup(new QActionGroup(this)),
    renderSettings(RenderSettings::getInstance()),
    preferences(new Preferences),
    materialBrowser(MaterialBrowser::getInstance()),
    movieMaker(MovieMaker::getInstance()),
    pw(nullptr),
    recent(nullptr)
{
    setCorner(Qt::TopLeftCorner, Qt::LeftDockWidgetArea);
    setCorner(Qt::TopRightCorner, Qt::RightDockWidgetArea);
    setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);
    setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);

    ui->setupUi(this);

    newProject();

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
        session->projectSettings->ui->spinBox_6->setMinimum(val);
        session->projectSettings->ui->spinBox_4->setMinimum(val);
        session->projectSettings->ui->spinBox_3->setValue(val, false);
    });

    connect(ui->spinBox_3, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), [this](int val) {
        ui->horizontalSlider_2->setMaximum(val);
        ui->spinBox_2->setMaximum(val);
        session->projectSettings->ui->spinBox_3->setMaximum(val);
        session->projectSettings->ui->spinBox_7->setMaximum(val);
        session->projectSettings->ui->spinBox_6->setValue(val, false);
    });

    /* connect actions */
    mappedSlot[ui->actionMove] = Camera::CA_Move;
    mappedSlot[ui->actionRotate] = Camera::CA_Rotate;
    mappedSlot[ui->actionScale] = Camera::CA_Scale;

    ui->actionMove->toggle();

    connect(ui->actionSettings, SIGNAL(triggered(bool)), renderSettings, SLOT(show()));

    connect(ui->actionPreferences, SIGNAL(triggered(bool)), preferences, SLOT(show()));

    connect(ui->actionProject_Settings, &QAction::triggered, [this] {
        ui->stackedWidget->setCurrentWidget(session->projectSettings);
        ui->dockWidget_2->show();
    });

    connect(ui->actionMaterial_Browser, SIGNAL(triggered(bool)), materialBrowser, SLOT(show()));
    connect(ui->actionContent_Browser, SIGNAL(triggered(bool)), materialBrowser, SLOT(show()));

    auto *ag = new QActionGroup(this);
    ag->addAction(ui->actionSimple);
    ag->addAction(ui->actionCycle);

    connect(session->projectSettings->ui->spinBox, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), [this] (int value) {
        timer.setInterval(1000 / value);
    });

    timer.setInterval(1000 / session->projectSettings->ui->spinBox->value());

    connect(session->projectSettings->ui->spinBox_3, SIGNAL(valueChanged(int)), ui->spinBox_2, SLOT(setValue(int)));
    connect(session->projectSettings->ui->spinBox_4, SIGNAL(valueChanged(int)), this, SLOT(setSoftMinimum(int)));
    connect(session->projectSettings->ui->spinBox_5, SIGNAL(valueChanged(int)), this, SLOT(setFrame(int)));
    connect(session->projectSettings->ui->spinBox_6, SIGNAL(valueChanged(int)), ui->spinBox_3, SLOT(setValue(int)));
    connect(session->projectSettings->ui->spinBox_7, SIGNAL(valueChanged(int)), this, SLOT(setSoftMaximum(int)));

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

    addAction(ui->actionViewport);

    connect(ui->actionViewport, &QAction::triggered, [this] {
        ui->stackedWidget->setCurrentWidget(session->viewport);
        ui->dockWidget_2->show();
    });

    connect(ui->record, &MediaControl::toggled, [this](bool checked) {
        ui->canvas->setStyleSheet(checked ? "background: #d40000;" : "background: #4d4d4d;");
        Camera::setAutomaticKeyframing(checked);
    });

    connect(ui->key, &MediaControl::clicked, [this] {
        session->currentCamera->captureFrame();
        ui->horizontalSlider->update();
    });

    connect(ui->actionFocus, &QAction::triggered, [this] {
        session->currentCamera->callibrate(qobject_cast<TreeModel*>(session->simulation->getModel())->getSelected());
    });

    connect(ui->actionSelect, &QAction::toggled, [this](bool checked) {
        ui->stackedWidget_2->setAttribute(Qt::WA_TransparentForMouseEvents, checked);
        Selection::setSelectionType(checked ? RECTANGULAR_SELECTION : NO_SELECTION);
    });

    connect(ui->actionCustom, &QAction::toggled, [this](bool checked) {
        ui->stackedWidget_2->setAttribute(Qt::WA_TransparentForMouseEvents, checked);
        Selection::setSelectionType(checked ? CUSTOM_SHAPE_SELECTION : NO_SELECTION);
    });

    ui->plot->followSlider(ui->horizontalSlider);

    connect(materialBrowser, &MaterialBrowser::materialsSelected, [this](const QList<Material*>& selected) {
        ui->page_7->handleSelection(selected);
        ui->stackedWidget->setCurrentWidget(ui->page_7);
    });

    connect(ui->page_7, SIGNAL(attributesChanged(const Material*)), materialBrowser, SLOT(update()));

    connect(ui->actionCamera, &QAction::triggered, [this] {
        auto camera = new Camera(*session->currentCamera);

        addCamera(camera);
        qobject_cast<TreeModel*>(session->treeView->model())->addCamera(camera);
    });

    connect(&PickWidget::getSignalMapper(), static_cast<void(QSignalMapper::*)(QWidget *)>(&QSignalMapper::mapped), [this](QObject *object) {
        pw = qobject_cast<PickWidget*>(object);
        ui->statusBar->showPermanentMessage("Pick mode: Click on an object, material, tag ...");
        QApplication::setOverrideCursor(Qt::WhatsThisCursor);
    });

    connect(ui->page_2, SIGNAL(attributeChanged()), ui->scene, SLOT(update()));

    connect(movieMaker, &MovieMaker::progressChanged, ui->statusBar, &StatusBar::setProgress);

    connect(ui->scene, &VizWidget::selectionChanged, this, &MainWindow::handleSceneSelection);

    connect(ui->page_7, SIGNAL(attributesChanged(const Material*)), ui->scene, SLOT(update()));

    connect(ui->page_5, &CameraAttributes::selected, [this](const QPersistentModelIndex& index) {
        session->treeView->selectionModel()->select(index, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

#include "session.h"

bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{
    if (pw && event->type() == QEvent::MouseButtonPress)
    {
        QApplication::restoreOverrideCursor();
        ui->statusBar->clearPermanentMessage();

        auto p = session->treeView->mapFromGlobal(reinterpret_cast<QMouseEvent*>(event)->globalPos());

        if (session->treeView->rect().contains(p))
        {
            pw->pick(session->treeView->pick(p));
            pw = nullptr;
            return true;
        }

        auto q = ui->scene->mapFromGlobal(reinterpret_cast<QMouseEvent*>(event)->globalPos());

        if (ui->scene->rect().contains(q))
        {
            pw->pick(ui->scene->pick(q));
            pw = nullptr;
            return true;
        }

        pw->pick(QModelIndex());
        pw = nullptr;
    }

    if (Draggable::pressedButton() != Qt::NoButton)
    {
        if (event->type() == QEvent::MouseButtonPress && event->spontaneous())
            return true;

        if (event->type() == QEvent::MouseButtonRelease && reinterpret_cast<QMouseEvent*>(event)->button() != Draggable::pressedButton())
            return true;
    }

    if (watched == ui->scene && event->type() == QEvent::Wheel)
    {
        QApplication::sendEvent(session->currentCamera, event);
        return true;
    }

    return QObject::eventFilter(watched, event);
}

#include <QJsonObject>

void MainWindow::read(const QJsonObject &json, Session* s)
{
    const QJsonObject children = json["Descendants"].toObject();

    for (auto child = children.end() - 1; child != children.begin() - 1; child--)
    {
        const QJsonObject object = child.value().toObject()["Object"].toObject();

        if (object["class"] == "Layer")
        {
            auto simulationLayer = std::make_shared<SimulationLayerConcatenation>();
            simulationLayer->read(object["paths"].toArray());

            s->simulation->addSimulationLayerConcatenation(simulationLayer, false);
        }

        if (object["class"] == "Camera")
        {
            auto camera = new Camera(s);
            camera->read(object);

            addCamera(camera);
            qobject_cast<TreeModel*>(s->treeView->model())->addCamera(camera);
        }
    }
}

Session* MainWindow::makeSession()
{
    auto s = new Session();

    // add action to menu
    ui->menuWindows->addAction(s->action);
    connect(s->action, &QAction::triggered, [=]() {
        setCurrentSession(s);
    });

    // connect tree model with GUI
    connect(s->simulation->getModel(), &TreeModel::propertyChanged, [this] {
        session->treeView->update();
        ui->scene->update();
    });

    // add tree view to objects
    ui->stackedWidget_3->addWidget(s->treeView);
    connect(s->treeView, &TreeView::cameraChanged, this, &MainWindow::changeCamera);
    connect(s->treeView->selectionModel(), &QItemSelectionModel::selectionChanged, this, &MainWindow::handleModelSelection);

    // add project settings to attributes
    ui->stackedWidget->addWidget(s->projectSettings);

    // add viewport settings to attributes
    ui->stackedWidget->addWidget(s->viewport);
    connect(s->viewport, SIGNAL(viewportChanged()), ui->scene, SLOT(update()));

    // add editor camera to available cameras
    addCamera(s->editorCamera);

    //
    connect(s->simulation, SIGNAL(frameCountChanged(int)), this, SLOT(updateFrameCount(int)));
    //

    return s;
}

void MainWindow::newProject()
{
    setCurrentSession(makeSession());

    //
    ui->plot->setRange(0, 0);
    ui->horizontalSlider->setRange(0, 0);
    ui->horizontalSlider_2->setRange(0, 0);

    lastFrame = 0;

    softMinimum = 0;
    softMaximum = 0;
    //

    ui->stackedWidget->setCurrentWidget(session->projectSettings);
    ui->dockWidget_2->show();
}

void MainWindow::setCurrentSession(Session *s)
{
    session = s;

    // update window title
    setWindowTitle(QString("QChromosome 4D Studio - [%1]").arg(session->projectSettings->getFileName()));

    // update objects
    ui->stackedWidget_3->setCurrentWidget(session->treeView);

    // clear attributes
    MetaAttributes::setSession(session);
    ui->stackedWidget->setCurrentWidget(ui->page_9);

    // update camera
    changeCamera(session->currentCamera);

    // update view
    ui->scene->setSession(session);

    // update materials
    materialBrowser->setSession(session);

    //
    ui->plot->setSimulation(s->simulation);
    //
}

#include <QStandardPaths>

void MainWindow::openProject()
{
    QString path = QFileDialog::getOpenFileName(0, "Open...", QStandardPaths::writableLocation(QStandardPaths::HomeLocation), QString("QChromosome 4D Project File (*.%1)").arg(ProjectSettings::suffix));

    if (!path.isEmpty())
    {
        auto s = makeSession();

        QFile file(path);
        file.open(QIODevice::ReadOnly | QIODevice::Text);
        const QJsonObject project = QJsonDocument::fromJson(file.readAll()).object();
        file.close();

        const QJsonObject viewport = project["Viewport"].toObject();
        s->viewport->read(viewport);

        const QJsonObject camera = project["Camera"].toObject();
        s->editorCamera->read(camera);

        const QJsonArray materials = project["Materials"].toArray();
        qobject_cast<MaterialListModel*>(s->listView->model())->read(materials);

        const QJsonObject objects = project["Objects"].toObject();
        read(objects, s);
        s->simulation->getModel()->read(objects);

        const QJsonObject projectSettings = project["Project Settings"].toObject();
        s->projectSettings->read(projectSettings);

        s->projectSettings->filePath.setFile(path);
        s->projectSettings->ui->lineEdit_6->setText(path);

        setCurrentSession(s);
    }
}

#include "importdialog.h"

void MainWindow::addLayer()
{
    try {
        QString path = QFileDialog::getOpenFileName(0, "Import...", QSettings().value("locallib").toString(), "All QChromosome 4D Files (*.pdb *.bin);;RCSB Protein Data Bank (*.pdb);;Motions (*.bin)");

        if (!path.isEmpty())
        {
            int offset = session->atomBuffer.size();

            std::shared_ptr<SimulationLayer> simulationLayer;

            if (path.endsWith(".pdb"))
                simulationLayer = std::make_shared<SimulationLayer>(std::make_shared<PDBSimulationLayer>(path.toStdString()));
            else
                simulationLayer = std::make_shared<SimulationLayer>(std::make_shared<ProtobufSimulationLayer>(path.toStdString()));

            ImportDialog impd(simulationLayer.get(), this);
            impd.setWindowTitle(QString("Import - [%1]").arg(QFileInfo(path).fileName()));

            if (impd.exec() == QDialog::Accepted)
            {
                setFrame(0);
                session->simulation->addSimulationLayerConcatenation(std::make_shared<SimulationLayerConcatenation>(simulationLayer));

                ui->scene->update();
                ui->plot->updateSimulation();

                session->currentCamera->callibrate(session->atomBuffer.mid(offset));
            }
        }
    } catch (std::exception& e) {
        QMessageBox::critical(0, "Error occured.", e.what());
    }
}

void MainWindow::saveProject()
{
    if (session->projectSettings->getSaveFileName())
    {
        QJsonObject project;
        session->write(project);

        session->projectSettings->writeSaveFile(QJsonDocument(project));
    }
}

void MainWindow::saveProjectAs()
{
    if (session->projectSettings->getNewSaveFileName())
    {
        QJsonObject project;
        session->write(project);

        session->projectSettings->writeSaveFile(QJsonDocument(project));
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
    session->projectSettings->ui->spinBox_5->setMaximum(lastFrame);
    session->projectSettings->ui->spinBox_6->setMaximum(lastFrame);

    if (expandRange)
        ui->spinBox_3->setValue(lastFrame);

    if (expandInterval)
        ui->horizontalSlider_2->setUpperBound(lastFrame);
}

void MainWindow::setFrame(int n)
{
    ui->horizontalSlider->setValue(n);
    ui->spinBox->setValue(n);
    ui->scene->update();
    ui->plot->setValue(n);
    SplineInterpolator::setFrame(n);
    session->setFrame(session->simulation->getFrame(n));
    session->projectSettings->ui->spinBox_5->setValue(n, false);
}

void MainWindow::setSoftMinimum(int min)
{
    ui->horizontalSlider->setSoftMinimum(min);
    ui->horizontalSlider_2->setLowerBound(min, false);
    ui->plot->setSoftMinimum(min);
    session->projectSettings->ui->spinBox_7->setMinimum(min);
    session->projectSettings->ui->spinBox_4->setValue(min, false);

    softMinimum = min;
}

void MainWindow::setSoftMaximum(int max)
{
    ui->horizontalSlider->setSoftMaximum(max);
    ui->horizontalSlider_2->setUpperBound(max, false);
    ui->plot->setSoftMaximum(max);
    session->projectSettings->ui->spinBox_4->setMaximum(max);
    session->projectSettings->ui->spinBox_7->setValue(max, false);

    softMaximum = max;
}

void MainWindow::start()
{
    setFrame(0);
}

void MainWindow::previous()
{
    frameNumber_t previousFrame = session->simulation->getPreviousTime(session->projectSettings->getDocumentTime());

    if (session->projectSettings->getDocumentTime() > 0)
        setFrame(previousFrame);
}

void MainWindow::reverse_previous()
{
    qint64 previousFrame = qMax(session->projectSettings->getDocumentTime() - qRound(1. * time.restart() * session->projectSettings->ui->spinBox->value() / 1000), 0);

    if (session->projectSettings->getDocumentTime() > (ui->actionPreview_range->isChecked() ? softMinimum : 0))
        setFrame(previousFrame);
    else
    {
        if (ui->actionSimple->isChecked())
            ui->reverse->click();
        else
            setFrame(ui->actionPreview_range->isChecked() ? softMaximum : lastFrame);
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

        if (ui->actionPreview_range->isChecked() && (session->projectSettings->getDocumentTime() <= softMinimum || session->projectSettings->getDocumentTime() > softMaximum))
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

        if (ui->actionPreview_range->isChecked() && (session->projectSettings->getDocumentTime() < softMinimum || session->projectSettings->getDocumentTime() >= softMaximum))
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
    frameNumber_t nextFrame = session->simulation->getNextTime(session->projectSettings->getDocumentTime());
    session->simulation->getFrame(nextFrame);

    if (session->projectSettings->getDocumentTime() < lastFrame)
        setFrame(nextFrame);
}

void MainWindow::play_next()
{
    qint64 nextFrame = session->projectSettings->getDocumentTime() + qRound(1. * time.restart() * session->projectSettings->ui->spinBox->value() / 1000);

    if (session->projectSettings->getDocumentTime() < (ui->actionPreview_range->isChecked() ? softMaximum : lastFrame))
        setFrame(nextFrame);
    else
    {
        if (ui->actionSimple->isChecked())
            ui->play->click();
        else
            setFrame(ui->actionPreview_range->isChecked() ? softMinimum : 0);
    }

    session->simulation->getFrame(session->projectSettings->getDocumentTime()+1);//TODO paskudny hack, usunąć po dodaniu wątku
}

void MainWindow::end()
{
    setFrame(lastFrame);
    session->simulation->getFrame(lastFrame+1);//TODO paskudny hack, usunąć po dodaniu wątku
}

void MainWindow::selectAll()
{
    session->treeView->selectAll();
}

void MainWindow::handleSceneSelection(const QItemSelection&selected, QItemSelectionModel::SelectionFlags flags)
{
    session->treeView->selectionModel()->select(selected, flags | QItemSelectionModel::Rows);
}

void MainWindow::handleModelSelection(const QItemSelection& selected, const QItemSelection& deselected)
{
    auto model = qobject_cast<TreeModel*>(session->simulation->getModel());

    model->setSelected(deselected.indexes(), false);
    model->setSelected(selected.indexes(), true);

    if (recent) recent->unsetSelection();

    if (!session->treeView->selectionModel()->hasSelection())
    {
        recent = nullptr;
        ui->stackedWidget->setCurrentWidget(ui->page_9);
        return;
    }

    Camera::setOrigin(model->getOrigin());

    QModelIndexList selectedRows = session->treeView->selectionModel()->selectedRows();

    auto index = selectedRows.first();

    int selectionType = index.sibling(index.row(), 1).data().toInt();

    for (const auto& r : selectedRows)
        if (selectionType != r.sibling(r.row(), 1).data().toInt())
        {
            selectionType = -1;
            break;
        }

    static const QMap<int, MetaAttributes*> map({{CameraObject, ui->page_5},{AtomObject, ui->page_2},{LayerObject, ui->page_3}});

    auto i = map.find(selectionType);
    recent = i == map.end() ? ui->page_8 : *i;

    recent->setSelection(session->simulation->getModel(), selectedRows);
    ui->stackedWidget->setCurrentWidget(recent);
    ui->dockWidget_2->show();
}

void MainWindow::setBaseAction(bool enabled)
{
    if (enabled)
    {
        modifiers.last() = qobject_cast<QAction*>(sender());
        Camera::setCurrentAction(mappedSlot[sender()]);
    }
}

void MainWindow::capture() const
{
    QString suffix = renderSettings->timestamp() ? QDateTime::currentDateTime().toString("yyyy'-'MM'-'dd'T'HH'-'mm'-'ss") : "";
    movieMaker->captureScene1(session->projectSettings->getDocumentTime(), session->simulation, session->currentCamera, suffix);
}

void MainWindow::captureMovie() const
{
    QString suffix = renderSettings->timestamp() ? QDateTime::currentDateTime().toString("yyyy'-'MM'-'dd'T'HH'-'mm'-'ss") : "";
    movieMaker->captureScene(ui->horizontalSlider_2->getLowerBound(), ui->horizontalSlider_2->getUpperBound(), session->simulation, session->currentCamera, suffix, session->projectSettings->ui->spinBox->value());
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
    switch (QMessageBox::question(0, "QChromosome 4D Studio", QString("Do you want to save the changes to the project \"%1\" before quitting?").arg(session->projectSettings->getFileName()), QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel, QMessageBox::Yes))
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

    connect(camera, SIGNAL(modelViewChanged(QMatrix4x4,QObject*)), ui->scene, SLOT(update()));
    connect(camera, SIGNAL(projectionChanged(QMatrix4x4,QObject*)), ui->scene, SLOT(update()));
    connect(renderSettings, &RenderSettings::aspectRatioChanged, camera, &Camera::setAspectRatio);
    connect(camera, &SplineInterpolator::selectionChanged, [=] {
        ui->page_6->setSplineInterpolator(camera);
        ui->stackedWidget->setCurrentWidget(ui->page_6);
        ui->dockWidget_2->show();
    });

    ui->stackedWidget_2->addWidget(camera);
}

void MainWindow::changeCamera(Camera *camera)
{
    if (!camera)
        camera = session->editorCamera;

    ui->stackedWidget_2->currentWidget()->blockSignals(true);
    camera->blockSignals(false);
    ui->stackedWidget_2->setCurrentWidget(camera);

    ui->horizontalSlider->setSplineInterpolator(camera);
}
