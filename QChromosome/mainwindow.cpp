#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ui_projectsettings.h"
#include "ui_mediapanel.h"

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
    recent(nullptr),
    session(nullptr)
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
    connect(s->projectSettings, &ProjectSettings::fileNameChanged, [this](const QString& fileName) {
        setWindowTitle(QString("QChromosome 4D Studio - [%1]").arg(fileName));
    });

    // add viewport settings to attributes
    ui->stackedWidget->addWidget(s->viewport);
    connect(s->viewport, SIGNAL(viewportChanged()), ui->scene, SLOT(update()));

    // add editor camera to available cameras
    addCamera(s->editorCamera);

    // add media panel to available panels
    ui->stackedWidget_4->addWidget(s->mediaPanel);

    //
    connect(s->mediaPanel->ui->record, &MediaControl::toggled, [this](bool checked) {
        ui->canvas->setStyleSheet(checked ? "background: #d40000;" : "background: #4d4d4d;");
        Camera::setAutomaticKeyframing(checked);
    });
    //

    s->blockSignals(true);
    connect(s, &Session::documentTimeChanged, this, &MainWindow::setFrame);

    return s;
}

void MainWindow::newProject()
{
    setCurrentSession(makeSession());

    //
    ui->plot->setRange(0, 0);
    //

    ui->stackedWidget->setCurrentWidget(session->projectSettings);
    ui->dockWidget_2->show();
}

void MainWindow::setCurrentSession(Session *s)
{
    if (session)
        session->blockSignals(true);

    session = s;

    // update window title
    setWindowTitle(QString("QChromosome 4D Studio - [%1]").arg(session->projectSettings->getFileName()));

    // reallocate OpenGL buffers
    session->reallocateBuffers();

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

    // update media panel
    ui->stackedWidget_4->setCurrentWidget(session->mediaPanel);

    //
    ui->plot->setSimulation(s->simulation);
    ui->plot->followSlider(s->mediaPanel->ui->horizontalSlider);
    //

    session->blockSignals(false);
    session->setDocumentTime(session->projectSettings->getDocumentTime());
}

void MainWindow::openProject()
{
    auto s = makeSession();

    if (s->openProject())
    {
        for (Camera* camera : s->userCameras)
            addCamera(camera);

        setCurrentSession(s);

        ui->stackedWidget->setCurrentWidget(session->projectSettings);
        ui->dockWidget_2->show();
    }
    else
        delete s;
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
                session->setDocumentTime(0);
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
    session->saveProject();
}

void MainWindow::saveProjectAs()
{
    session->saveProjectAs();
}

void MainWindow::setFrame(int n)
{
    ui->scene->update();
    SplineInterpolator::setFrame(n);
    //
    ui->plot->setValue(n);
    //
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
    movieMaker->captureScene1(session, suffix);
}

void MainWindow::captureMovie() const
{
    QString suffix = renderSettings->timestamp() ? QDateTime::currentDateTime().toString("yyyy'-'MM'-'dd'T'HH'-'mm'-'ss") : "";
    movieMaker->captureScene(session->mediaPanel->ui->horizontalSlider_2->getLowerBound(), session->mediaPanel->ui->horizontalSlider_2->getUpperBound(), session, suffix);
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

    connect(camera, SIGNAL(modelViewChanged(QMatrix4x4)), ui->scene, SLOT(update()));
    connect(camera, SIGNAL(projectionChanged(QMatrix4x4)), ui->scene, SLOT(update()));
    connect(renderSettings, &RenderSettings::aspectRatioChanged, camera, &Camera::setAspectRatio);
    connect(camera, &SplineInterpolator::selectionChanged, [=] {
        if (camera->hasSelection())
        {
            ui->page_6->setSplineInterpolator(camera);
            ui->stackedWidget->setCurrentWidget(ui->page_6);
            ui->dockWidget_2->show();
        }
        else
        {
            ui->stackedWidget->setCurrentWidget(ui->page_9);
            ui->dockWidget_2->show();
        }
    });

    ui->stackedWidget_2->addWidget(camera);
}

void MainWindow::changeCamera(Camera *camera)
{
    session->changeCamera(camera);
    ui->stackedWidget_2->setCurrentWidget(session->currentCamera);
}
