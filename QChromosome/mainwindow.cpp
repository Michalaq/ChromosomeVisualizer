#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ui_projectsettings.h"

#include "../QtChromosomeViz_v2/bartekm_code/PDBSimulationLayer.h"
#include "../QtChromosomeViz_v2/bartekm_code/ProtobufSimulationlayer.h"
#include "visibilitydelegate.h"
#include "namedelegate.h"
#include "tagsdelegate.h"
#include <QtConcurrent/QtConcurrentRun>

static const char * ext = ".qcs";

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    actionGroup(new QActionGroup(this)),
    renderSettings(RenderSettings::getInstance()),
    preferences(new Preferences),
    materialBrowser(MaterialBrowser::getInstance()),
    pw(nullptr),
    msg(new QLabel("Pick mode: Click on an object, material, tag ...")),
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

    connect(ui->spinBox_2, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), [this](int val) {
        session->PS_setMinimumTime(val);
    });

    connect(ui->spinBox_3, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), [this](int val) {
        session->PS_setMaximumTime(val);
    });

    /* connect actions */
    mappedSlot[ui->actionMove] = Camera::CA_Move;
    mappedSlot[ui->actionRotate] = Camera::CA_Rotate;
    mappedSlot[ui->actionScale] = Camera::CA_Scale;

    ui->actionMove->toggle();

    connect(ui->actionSettings, SIGNAL(triggered(bool)), renderSettings, SLOT(show()));

    connect(ui->actionPreferences, SIGNAL(triggered(bool)), preferences, SLOT(show()));

    connect(ui->actionProject_Settings, &QAction::triggered, [this] {
        ui->stackedWidget->setCurrentIndex(0);
        ui->dockWidget_2->show();
    });

    connect(ui->actionMaterial_Browser, SIGNAL(triggered(bool)), materialBrowser, SLOT(show()));
    connect(ui->actionContent_Browser, SIGNAL(triggered(bool)), materialBrowser, SLOT(show()));

    auto *ag = new QActionGroup(this);
    ag->addAction(ui->actionSimple);
    ag->addAction(ui->actionCycle);

    timer.setInterval(1000 / session->PS_getFPS());

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
        ui->stackedWidget->setCurrentIndex(3);
        ui->dockWidget_2->show();
    });

    ui->scene->setViewport(ui->page_4);
    Camera::setViewport(ui->page_4);

    connect(ui->page_4, &Viewport::viewportChanged, [this] {
        session->currentCamera->update();
        ui->scene->update();
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
        session->currentCamera->setLookAt(Camera::getOrigin());
    });

    connect(ui->actionSelect, &QAction::toggled, [this](bool checked) {
        ui->stackedWidget_2->setAttribute(Qt::WA_TransparentForMouseEvents, checked);
        Selection::setSelectionType(checked ? RECTANGULAR_SELECTION : NO_SELECTION);
    });

    connect(ui->actionCustom, &QAction::toggled, [this](bool checked) {
        ui->stackedWidget_2->setAttribute(Qt::WA_TransparentForMouseEvents, checked);
        Selection::setSelectionType(checked ? CUSTOM_SHAPE_SELECTION : NO_SELECTION);
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

    connect(materialBrowser, &MaterialBrowser::materialsSelected, [this](const QList<Material*>& selected) {
        ui->page_7->handleSelection(selected);
        ui->stackedWidget->setCurrentIndex(6);
    });

    connect(ui->page_7, SIGNAL(attributesChanged(const Material*)), materialBrowser, SLOT(update()));

    connect(ui->actionCamera, &QAction::triggered, [this] {
        addCamera(new Camera(*session->currentCamera, session));
    });

    connect(ui->treeView, &TreeView::cameraChanged, [this](Camera* camera) {
        if (!camera) camera = session->editorCamera;
        session->currentCamera->blockSignals(true);
        camera->blockSignals(false);
        ui->stackedWidget_2->setCurrentWidget(camera);
        ui->horizontalSlider->setSplineInterpolator(camera);
        session->currentCamera = camera;
    });

    msg->setStyleSheet("padding-left: 1px; padding-top: -1px; color: white;");
    msg->hide();

    statusBar()->addPermanentWidget(msg, 1);

    connect(&PickWidget::getSignalMapper(), static_cast<void(QSignalMapper::*)(QWidget *)>(&QSignalMapper::mapped), [this](QObject *object) {
        pw = qobject_cast<PickWidget*>(object);
        msg->show();
        QApplication::setOverrideCursor(Qt::WhatsThisCursor);
    });

    connect(ui->page_5, &CameraAttributes::selected, [this](const QPersistentModelIndex& index) {
        ui->treeView->selectionModel()->select(index, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
    });

    connect(ui->page_2, SIGNAL(attributeChanged()), ui->scene, SLOT(update()));

    connect(ui->spinBox, QOverload<int>::of(&QSpinBox::valueChanged), [this](int value) {
        session->PS_setDocumentTime(value);
    });

    connect(ui->plot, &Plot::valueChanged, [this](int value) {
        session->PS_setDocumentTime(value);
    });

    connect(ui->horizontalSlider, &QSlider::valueChanged, [this](int value) {
        session->PS_setDocumentTime(value);
    });

    connect(ui->horizontalSlider_2, &RangeSlider::lowerBoundChanged, [this](int value) {
        session->PS_setPreviewMinTime(value);
    });

    connect(ui->horizontalSlider_2, &RangeSlider::upperBoundChanged, [this](int value) {
        session->PS_setPreviewMaxTime(value);
    });

    ui->treeView->header()->resizeSection(3, 40);
    ui->treeView->header()->setSectionResizeMode(3, QHeaderView::Fixed);
    ui->treeView->header()->setSectionResizeMode(5, QHeaderView::Fixed);

    connect(ui->scene, &VizWidget::selectionChanged, this, &MainWindow::handleSceneSelection);
}

MainWindow::~MainWindow()
{
    delete ui;
}

bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{
    if (pw && event->type() == QEvent::MouseButtonPress)
    {
        QApplication::restoreOverrideCursor();
        msg->hide();

        auto p = ui->treeView->mapFromGlobal(reinterpret_cast<QMouseEvent*>(event)->globalPos());

        if (ui->treeView->rect().contains(p))
        {
            pw->pick(ui->treeView->pick(p));
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

            session->simulation->addSimulationLayerConcatenation(simulationLayer, false);
        }

        if (object["class"] == "Camera")
        {
            auto camera = new Camera(session);
            camera->read(object);

            addCamera(camera);
        }
    }
}

void MainWindow::newProject()
{
    if (session) session->currentCamera->blockSignals(true);

    setSession(new Session(this));
    sessions.push(session);

    connect(session->editorCamera, &Camera::modelViewChanged, ui->scene, &VizWidget::setModelView);
    connect(session->editorCamera, &Camera::projectionChanged, ui->scene, &VizWidget::setProjection);
    connect(renderSettings, &RenderSettings::aspectRatioChanged, session->editorCamera, &Camera::setAspectRatio);
    connect(session->editorCamera, &SplineInterpolator::selectionChanged, [=] {
        ui->page_6->setSplineInterpolator(session->editorCamera);
        ui->stackedWidget->setCurrentIndex(5);
        ui->dockWidget_2->show();
    });

    connect(session->selectionModel, &QItemSelectionModel::selectionChanged, this, &MainWindow::handleModelSelection);

    connect(session->simulation->getModel(), &TreeModel::propertyChanged, [this] {
        ui->treeView->update();
        ui->scene->update();
    });

    connect(ui->page_7, SIGNAL(attributesChanged(const Material*)), session->simulation->getModel(), SLOT(updateAttributes(const Material*)));

    ui->menuWindows->addAction(session->getAction());
}

void MainWindow::openProject()
{
    QString path = QFileDialog::getOpenFileName(0, "", QStandardPaths::writableLocation(QStandardPaths::HomeLocation), QString("QChromosome 4D Project File (*%1)").arg(ext));

    if (!path.isEmpty())
    {
        newProject();

        session->I_setFilePath(path);

        QFile file(path);
        file.open(QIODevice::ReadOnly | QIODevice::Text);
        const QJsonObject project = QJsonDocument::fromJson(file.readAll()).object();
        file.close();

        const QJsonObject viewport = project["Viewport"].toObject();
        ui->page_4->read(viewport);

        const QJsonObject camera = project["Camera"].toObject();
        session->editorCamera->read(camera);

        const QJsonArray materials = project["Materials"].toArray();
        materialBrowser->read(materials);

        const QJsonObject objects = project["Objects"].toObject();
        read(objects);
        session->simulation->getModel()->read(objects);

        const QJsonObject projectSettings = project["Project Settings"].toObject();
        session->PS_read(projectSettings);

        session->setSaved();
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

            session->simulation->addSimulationLayerConcatenation(std::make_shared<SimulationLayerConcatenation>(simulationLayer));

            session->PS_setDocumentTime(currentFrame);
        }
    } catch (std::exception& e) {
        QMessageBox::critical(0, "Error occured.", e.what());
    }
}

void MainWindow::saveProject()
{
    if (session->I_getFilePath().isEmpty())
        saveProjectAs();
    else
    {
        QJsonObject project;

        session->I_setFileFormat(QString("QChromosome 4D File (*%1)").arg(ext));
        session->I_setFileVersion("0.00");

        QJsonObject projectSettings;
        session->PS_write(projectSettings);
        project["Project Settings"] = projectSettings;

        QJsonObject viewport;
        ui->page_4->write(viewport);
        project["Viewport"] = viewport;

        QJsonObject camera;
        session->editorCamera->write(camera);
        project["Camera"] = camera;

        QJsonArray materials;
        materialBrowser->write(materials);
        project["Materials"] = materials;

        QJsonObject objects;
        session->simulation->getModel()->write(objects);
        project["Objects"] = objects;

        QFile file(session->I_getFilePath());
        file.open(QIODevice::WriteOnly | QIODevice::Text);
        file.write(QJsonDocument(project).toJson());
        file.close();

        session->setSaved();
    }
}

void MainWindow::saveProjectAs()
{
    QString path = QFileDialog::getSaveFileName(0, "", session->I_getFilePath().isEmpty() ? QStandardPaths::writableLocation(QStandardPaths::HomeLocation) : session->I_getFilePath(), QString("QChromosome 4D Project File (*%1)").arg(ext));

    if (!path.isEmpty())
    {
        QFileInfo info(path);
        path = info.dir().filePath(info.completeBaseName().append(ext));

        session->I_setFilePath(path);

        saveProject();
    }
}

void MainWindow::start()
{
    session->PS_setDocumentTime(0);
}

void MainWindow::previous()
{
    frameNumber_t previousFrame = session->simulation->getPreviousTime(currentFrame);

    if (currentFrame > 0)
        session->PS_setDocumentTime(previousFrame);
}

void MainWindow::reverse_previous()
{
    qint64 previousFrame = qMax(currentFrame - qRound(1. * time.restart() * session->PS_getFPS() / 1000), 0);

    if (currentFrame > (ui->actionPreview_range->isChecked() ? softMinimum : 0))
        session->PS_setDocumentTime(previousFrame);
    else
    {
        if (ui->actionSimple->isChecked())
            ui->reverse->click();
        else
            session->PS_setDocumentTime(ui->actionPreview_range->isChecked() ? softMaximum : lastFrame);
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
            session->PS_setDocumentTime(softMaximum);

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
            session->PS_setDocumentTime(softMinimum);

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
    frameNumber_t nextFrame = session->simulation->getNextTime(currentFrame);
    session->simulation->getFrame(nextFrame);

    if (currentFrame < lastFrame)
        session->PS_setDocumentTime(nextFrame);
}

void MainWindow::play_next()
{
    qint64 nextFrame = currentFrame + qRound(1. * time.restart() * session->PS_getFPS() / 1000);

    if (currentFrame < (ui->actionPreview_range->isChecked() ? softMaximum : lastFrame))
        session->PS_setDocumentTime(nextFrame);
    else
    {
        if (ui->actionSimple->isChecked())
            ui->play->click();
        else
            session->PS_setDocumentTime(ui->actionPreview_range->isChecked() ? softMinimum : 0);
    }

    session->simulation->getFrame(currentFrame+1);//TODO paskudny hack, usunąć po dodaniu wątku
}

void MainWindow::end()
{
    session->PS_setDocumentTime(lastFrame);
    session->simulation->getFrame(lastFrame+1);//TODO paskudny hack, usunąć po dodaniu wątku
}

void MainWindow::selectAll()
{
    ui->treeView->selectAll();
}

void MainWindow::handleSceneSelection(const QItemSelection&selected, QItemSelectionModel::SelectionFlags flags)
{
    ui->treeView->selectionModel()->select(selected, flags | QItemSelectionModel::Rows);
}

void MainWindow::handleModelSelection(const QItemSelection& selected, const QItemSelection& deselected)
{
    auto model = qobject_cast<TreeModel*>(session->simulation->getModel());

    model->setSelected(deselected.indexes(), false);
    model->setSelected(selected.indexes(), true);

    if (recent) recent->unsetSelection();

    if (!ui->treeView->selectionModel()->hasSelection())
    {
        recent = nullptr;
        Camera::setOrigin(QVector3D());
        ui->stackedWidget->setCurrentIndex(8);
        return;
    }

    QModelIndexList selectedRows = ui->treeView->selectionModel()->selectedRows();

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

#include "moviemaker.h"

void MainWindow::capture() const
{
    QString suffix = renderSettings->timestamp() ? QDateTime::currentDateTime().toString("yyyy'-'MM'-'dd'T'HH'-'mm'-'ss") : "";
    MovieMaker::captureScene1(currentFrame, session->simulation, ui->page_4, session->currentCamera, suffix);

    if (renderSettings->render() && renderSettings->openFile())
        QProcess::execute("xdg-open", {renderSettings->saveFile() + suffix + ".png"});
}

void MainWindow::captureMovie() const
{
    QString suffix = renderSettings->timestamp() ? QDateTime::currentDateTime().toString("yyyy'-'MM'-'dd'T'HH'-'mm'-'ss") : "";
    MovieMaker::captureScene(ui->horizontalSlider_2->getLowerBound(), ui->horizontalSlider_2->getUpperBound(), session->simulation, ui->page_4, session->currentCamera, suffix, session->PS_getFPS());

    if (renderSettings->render() && renderSettings->openFile())
        QProcess::execute("xdg-open", {renderSettings->saveFile() + suffix + ".mp4"});
}

void MainWindow::updateLocks()
{
    Camera::lockCoordinates(!ui->actionXLock->isChecked(), !ui->actionYLock->isChecked(), !ui->actionZLock->isChecked());
}

void MainWindow::setSession(Session *s)
{
    if (session) session->currentCamera->blockSignals(true);

    session = s;

    ui->plot->setSimulation(session->simulation);

    session->load();

    ui->treeView->setModel(session->simulation->getModel());
    ui->treeView->setSelectionModel(session->selectionModel);

    ui->treeView->hideColumn(1);
    ui->treeView->hideColumn(2);
    ui->treeView->hideColumn(4);
    ui->treeView->hideColumn(6);
    ui->treeView->setColumnWidth(3, 48);

    ui->stackedWidget->setCurrentIndex(0);

    ui->scene->setSession(session);
    ui->page->setSession(session);
    ui->scene->update();

    session->currentCamera->blockSignals(false);
    ui->stackedWidget_2->setCurrentWidget(session->currentCamera);

    ui->horizontalSlider->setSplineInterpolator(session->currentCamera);
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

bool MainWindow::closeCurrentSession()
{
    Session* tmp;

    switch (session->isSaved() ? QMessageBox::No : QMessageBox::question(0, "QChromosome 4D Studio", QString("Do you want to save the changes to the project \"%1\" before quitting?").arg(session->I_getFilePath().isEmpty() ? "Untitled" : QFileInfo(session->I_getFilePath()).fileName()), QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel, QMessageBox::Yes))
    {
    case QMessageBox::Yes:
        saveProject();
    case QMessageBox::No:
        if (sessions.size() == 1)
        {
            QApplication::quit();
            return false;
        }
        tmp = session;
        sessions.removeOne(tmp);
        setSession(sessions.top());
        delete tmp;
        return true;
    case QMessageBox::Cancel:
        return false;
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    while (closeCurrentSession());

    event->ignore();
}

void MainWindow::addCamera(Camera* camera)
{
    camera->blockSignals(true);

    connect(camera, &Camera::modelViewChanged, ui->scene, &VizWidget::setModelView);
    connect(camera, &Camera::projectionChanged, ui->scene, &VizWidget::setProjection);
    connect(renderSettings, &RenderSettings::aspectRatioChanged, camera, &Camera::setAspectRatio);
    connect(camera, &SplineInterpolator::selectionChanged, [=] {
        ui->page_6->setSplineInterpolator(camera);
        ui->stackedWidget->setCurrentIndex(5);
        ui->dockWidget_2->show();
    });

    ui->stackedWidget_2->addWidget(camera);

    ((TreeModel*)ui->treeView->model())->addCamera(camera);
}
