#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "session.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    actionGroup(new QActionGroup(this)),
    renderSettings(RenderSettings::getInstance()),
    preferences(Preferences::getInstance()),
    materialBrowser(MaterialBrowser::getInstance()),
    movieMaker(MovieMaker::getInstance()),
    pw(nullptr),
    recent(nullptr),
    session(nullptr),
    sessions(new QActionGroup(this)),
    light(new DefaultLight)
{
    setCorner(Qt::TopLeftCorner, Qt::LeftDockWidgetArea);
    setCorner(Qt::TopRightCorner, Qt::RightDockWidgetArea);
    setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);
    setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);

    ui->setupUi(this);

    separator = ui->menuCameras->insertSeparator(ui->actionPerspective);

    newProject();

    connect(ui->actionInfo, &QAction::triggered, [this] {
        QMessageBox::about(0, "About QChromosome 4D Studio", QString(
                           "<h3>QChromosome 4D Studio</h3>"
                           "<p>Version 1.0</p>"
                           "<p>License GNU Lesser General Public License (LGPL)</p>"
                           "<p>Based on Qt 5.9.1</p>"
                           "<p><small>Copyright &copy; 2018 Bartłomiej Zawalski &lt;<a href=\"mailto:%1\">%1</a>&gt;</small></p>"
                           ).arg("bz337736@students.mimuw.edu.pl"));
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

    modifiers.push_back(ui->actionRotate);

    /* connect actions */
    mappedSlot[ui->actionMove] = Camera::CA_Move;
    mappedSlot[ui->actionRotate] = Camera::CA_Rotate;
    mappedSlot[ui->actionScale] = Camera::CA_Scale;

    ui->actionRotate->toggle();

    connect(ui->actionRender_settings, SIGNAL(triggered(bool)), renderSettings, SLOT(show()));

    connect(ui->actionPreferences, SIGNAL(triggered(bool)), preferences, SLOT(show()));

    connect(ui->actionProject_Settings, &QAction::triggered, [this] {
        ui->stackedWidget->setCurrentWidget(session->projectSettings);
        ui->dockWidget_2->show();
    });

    connect(ui->actionContent_Browser, SIGNAL(triggered(bool)), materialBrowser, SLOT(show()));

    auto *ag = new QActionGroup(this);
    ag->addAction(ui->actionSimple);
    ag->addAction(ui->actionCycle);
    ag->addAction(ui->actionPing_Pong);

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

    connect(ui->actionConfigure, &QAction::triggered, [this] {
        ui->stackedWidget->setCurrentWidget(session->viewport);
        ui->dockWidget_2->show();
    });

    connect(ui->actionDefault_light, &QAction::triggered, light, &DefaultLight::show);

    connect(light, &DefaultLight::defaultLightChanged, [this](const QVector3D& direction) {
        session->viewportUniformBuffer[0].uvDefaultLight = direction;
        ui->scene->update();
    });

    connect(ui->actionFocus, &QAction::triggered, [this] {
        session->currentCamera->callibrate(0, true);
    });

    connect(ui->actionPivot, &QAction::triggered, [this] {
        session->setOrigin(0, true);
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
        session->simulation->getModel()->addCamera(camera);
    });

    connect(&PickWidget::getSignalMapper(), static_cast<void(QSignalMapper::*)(QWidget *)>(&QSignalMapper::mapped), [this](QObject *object) {
        pw = qobject_cast<PickWidget*>(object);
        ui->statusBar->showPermanentMessage("Pick mode: Click on an object, material, tag ...");
        QApplication::setOverrideCursor(Qt::WhatsThisCursor);
    });

    connect(ui->page_2, SIGNAL(attributeChanged()), ui->scene, SLOT(update()));
    connect(ui->page_3, SIGNAL(attributeChanged()), ui->scene, SLOT(update()));
    connect(ui->page_8, SIGNAL(attributeChanged()), ui->scene, SLOT(update()));

    connect(movieMaker, &MovieMaker::progressChanged, ui->statusBar, &StatusBar::setProgress);

    connect(ui->scene, &VizWidget::selectionChanged, this, &MainWindow::handleSceneSelection);

    connect(ui->page_7, SIGNAL(attributesChanged(const Material*)), ui->scene, SLOT(update()));

    connect(ui->page_5, &CameraAttributes::selected, [this](const QPersistentModelIndex& index) {
        session->treeView->selectionModel()->select(index, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
    });

    connect(ui->page_5, SIGNAL(modelViewChanged()), ui->scene, SLOT(update()));

    connect(ui->actionPreview_range, &QAction::triggered, [this](bool checked) {
        session->previewRange = checked;
    });

    connect(ui->actionSimple, &QAction::triggered, [this] {
        session->playMode = Session::PM_Simple;
    });

    connect(ui->actionCycle, &QAction::triggered, [this] {
        session->playMode = Session::PM_Cycle;
    });

    connect(ui->actionPing_Pong, &QAction::triggered, [this] {
        session->playMode = Session::PM_PingPong;
    });

    connect(preferences, &Preferences::coloringMethodChanged, [this](PointerToMemberFunction cm) {
        (session->simulation->getModel()->*cm)(QModelIndex());
        ui->scene->update();
    });

    ag = new QActionGroup(this);
    ag->addAction(ui->actionPerspective);
    ag->addAction(ui->actionParallel);

    connect(ui->actionPerspective, &QAction::triggered, [this] {
        session->currentCamera->setProjectionType(CP_Perspective);
    });

    connect(ui->actionParallel, &QAction::triggered, [this] {
        session->currentCamera->setProjectionType(CP_Parallel);
    });
}

MainWindow::~MainWindow()
{
    delete ui;
    delete light;
}

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
    auto s = new Session(this);

    // add action to menu
    ui->menuWindows->addAction(s->action);
    sessions->addAction(s->action);
    remaining.append(s);

    connect(s->action, &QAction::toggled, [=](bool checked) {
        if (checked)
        {
            remaining.removeOne(s);
            remaining.append(s);

            setCurrentSession(s);
        }
        else
        {
            if (s->playForwards)
            {
                playForwards(false);
                s->playForwards = true;
            }

            if (s->playBackwards)
            {
                playBackwards(false);
                s->playBackwards = true;
            }
        }
    });

    // connect tree model with GUI
    connect(s->simulation->getModel(), &TreeModel::propertyChanged, [this] {
        session->treeView->update();
        ui->scene->update();
    });
    connect(s->simulation->getModel(), &TreeModel::cameraChanged, this, &MainWindow::changeCamera);
    connect(s->simulation->getModel(), &TreeModel::tagSelected, [this](const QList<Material*>& selected) {
        ui->page_7->handleSelection(selected);
        ui->stackedWidget->setCurrentWidget(ui->page_7);
    });

    // add tree view to objects
    ui->stackedWidget_3->addWidget(s->treeView);
    connect(s->treeView->selectionModel(), &QItemSelectionModel::selectionChanged, [this] {
        ui->scene->update();
        handleModelSelection();
    });

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

    // add list of available cameras to menu View
    ui->menuCameras->insertMenu(separator, s->cameras);
    s->cameras->setStyleSheet(styleSheet());

    // add media panel to available panels
    ui->stackedWidget_4->addWidget(s->mediaPanel);

    // add plot to available plots
    ui->stackedWidget_5->addWidget(s->plot);

    // update attributes when document time changes
    connect(s, &Session::documentTimeChanged, ui->page_2, &AtomAttributes::updatePosition);

    connect(s, SIGNAL(documentTimeChanged(int)), ui->scene, SLOT(update()));

    // add render settings to stack
    renderSettings->addTabWidget(s->renderSettings);

    return s;
}

void MainWindow::newProject()
{
    makeSession()->action->trigger();

    ui->stackedWidget->setCurrentWidget(session->projectSettings);
    ui->dockWidget_2->show();
}

void MainWindow::setCurrentSession(Session *s)
{
    if (session)
        session->cameras->menuAction()->setVisible(false);

    session = s;

    session->cameras->menuAction()->setVisible(true);

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
    light->setDirection(session->viewportUniformBuffer[0].uvDefaultLight);

    // update materials
    materialBrowser->setSession(session);

    // update media panel
    ui->stackedWidget_4->setCurrentWidget(session->mediaPanel);

    static const QMap<Session::PlayMode, QAction*> map({{Session::PM_Simple, ui->actionSimple}, {Session::PM_Cycle, ui->actionCycle}, {Session::PM_PingPong, ui->actionPing_Pong}});

    ui->actionPreview_range->setChecked(session->previewRange);
    map[session->playMode]->setChecked(true);

    autokeying(session->autokeying);
    if (session->playForwards) playForwards(true);
    if (session->playBackwards) playBackwards(true);

    // update plot
    ui->stackedWidget_5->setCurrentWidget(session->plot);

    // update render settings
    renderSettings->setSession(session);
}

void MainWindow::openProject()
{
    auto s = makeSession();

    if (s->openProject())
    {
        for (Camera* camera : s->userCameras)
            addCamera(camera);

        s->action->trigger();

        ui->stackedWidget->setCurrentWidget(session->projectSettings);
        ui->dockWidget_2->show();
    }
    else
        delete s;
}

#include "simulation/pdbsimulationlayer.h"
#include "importdialog.h"

void MainWindow::addLayer()
{
    QString path = QFileDialog::getOpenFileName(0, "Import...", "", "All QChromosome 4D Files (*.pdb *.gz);;RCSB Protein Data Bank (*.pdb);;gzip archive (*.gz)");

    if (path.isEmpty())
        return;

    QFileInfo info(path);

    ImportDialog impd(this);
    impd.setWindowTitle(QString("Import - [%1]").arg(info.fileName()));

    if (impd.exec() != QDialog::Accepted)
        return;

    int offset = session->atomBuffer.size();

    SimulationLayer* layer = Q_NULLPTR;

    if (info.completeSuffix() == "pdb" || info.completeSuffix() == "pdb.gz")
        layer = new PDBSimulationLayer(path, session, impd.first(), impd.last(), impd.stride(), impd.loadInBackground());

    Q_ASSERT(layer);

    session->simulation->prepend(layer);
    (session->simulation->getModel()->*preferences->coloringMethod())(session->simulation->getModel()->index(0, 0));

    session->setDocumentTime(0);

    session->currentCamera->callibrate(offset, false);
    session->setOrigin(offset, false);

    session->plot->updateSimulation();
}

void MainWindow::saveProject()
{
    session->saveProject();
}

void MainWindow::saveProjectAs()
{
    session->saveProjectAs();
}

void MainWindow::selectAll()
{
    session->treeView->selectAll();
}

void MainWindow::handleSceneSelection(const QItemSelection&selected, QItemSelectionModel::SelectionFlags flags)
{
    session->treeView->selectionModel()->select(selected, flags | QItemSelectionModel::Rows);
}

void MainWindow::handleModelSelection()
{
    if (recent) recent->unsetSelection();

    if (!session->treeView->selectionModel()->hasSelection())
    {
        recent = nullptr;
        ui->stackedWidget->setCurrentWidget(ui->page_9);
        return;
    }

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

void MainWindow::updateLocks()
{
    Camera::lockCoordinates(!ui->actionXLock->isChecked(), !ui->actionYLock->isChecked(), !ui->actionZLock->isChecked());
}

void MainWindow::recordActiveObjects()
{
    session->currentCamera->captureFrame();
    session->mediaPanel->recordActiveObjects();
}

void MainWindow::autokeying(bool checked)
{
    session->autokeying = checked;
    session->mediaPanel->autokeying(checked);
    ui->actionAutokeying->setChecked(checked);
    ui->canvas->setStyleSheet(checked ? "background: #d40000;" : "background: #4d4d4d;");
}

void MainWindow::playForwards(bool checked)
{
    session->playForwards = checked;
    session->mediaPanel->playForwards(checked);
    ui->actionPlay_forwards->setChecked(checked);
}

void MainWindow::playBackwards(bool checked)
{
    session->playBackwards = checked;
    session->mediaPanel->playBackwards(checked);
    ui->actionPlay_backwards->setChecked(checked);
}

void MainWindow::goToStart()
{
    session->mediaPanel->goToStart();
}

void MainWindow::goToEnd()
{
    session->mediaPanel->goToEnd();
}

void MainWindow::goToNextFrame()
{
    session->mediaPanel->goToNextFrame();
}

void MainWindow::goToPreviousFrame()
{
    session->mediaPanel->goToPreviousFrame();
}

void MainWindow::render() const
{
    movieMaker->captureScene(session);
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

        ui->scene->makeCurrent();

        remaining.removeOne(session);
        delete session;
        session = nullptr;

        if (remaining.isEmpty())
            event->accept();
        else
        {
            remaining.last()->action->trigger();
            closeEvent(event);
        }

        break;

    case QMessageBox::Cancel:

        event->ignore();
        break;

    default:

        Q_ASSERT(false);
    }
}

void MainWindow::addCamera(Camera* camera)
{
    connect(camera, SIGNAL(modelViewChanged(QMatrix4x4)), ui->scene, SLOT(update()));
    connect(camera, SIGNAL(projectionChanged(QMatrix4x4)), ui->scene, SLOT(update()));
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
    connect(camera, &Camera::projectionTypeChanged, [this](Projection p) {
        switch (p)
        {
        case CP_Perspective:
            ui->actionPerspective->setChecked(true);
            break;
        case CP_Parallel:
            ui->actionParallel->setChecked(true);
            break;
        }
    });

    ui->stackedWidget_2->addWidget(camera);
}

void MainWindow::changeCamera(Camera *camera)
{
    session->changeCamera(camera);
    ui->stackedWidget_2->setCurrentWidget(session->currentCamera);

    switch (session->currentCamera->getProjectionType())
    {
    case CP_Perspective:
        ui->actionPerspective->setChecked(true);
        break;
    case CP_Parallel:
        ui->actionParallel->setChecked(true);
        break;
    }
}
