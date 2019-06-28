#include "session.h"

Session::Session(MainWindow* w) :
    QObject(),
    action(new QAction),
    simulation(new Simulation(this)),
    projectSettings(new ProjectSettings(this)),
    renderSettings(new TabWidget(this)),
    cameras(new QMenu("User cameras")),
    camerasGroup(new QActionGroup(cameras)),
    editorCamera(new Camera(this)),
    currentCamera(editorCamera),
    cameraUniformBuffer(editorCamera->id),
    treeView(new TreeView),
    mediaPanel(new MediaPanel(this, w)),
    chart(new QtCharts::QChart),
    plot(new Plot(this)),
    autokeying(false),
    playForwards(false),
    playBackwards(false),
    previewRange(false),
    playMode(PM_Cycle),
    chainCount(0),
    nd(new NameDelegate),
    vd(new VisibilityDelegate),
    td(new TagsDelegate),
    md(new MaterialDelegate),
    lastFrame(0)
{
    treeView->setMouseTracking(true);
    treeView->setAcceptDrops(true);
    treeView->setAlternatingRowColors(true);
    treeView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    treeView->setUniformRowHeights(true);
    treeView->setHeaderHidden(true);

    treeView->setModel(simulation->getModel());

    treeView->hideColumn(1);
    treeView->hideColumn(2);
    treeView->hideColumn(4);
    treeView->hideColumn(6);

    treeView->setItemDelegateForColumn(0, nd);
    treeView->setItemDelegateForColumn(3, vd);
    treeView->setItemDelegateForColumn(5, td);

    auto header = treeView->header();
    header->resizeSection(3, 42);
    header->setSectionResizeMode(3, QHeaderView::Fixed);
    header->setSectionResizeMode(5, QHeaderView::Fixed);

    listView = MaterialBrowser::getInstance()->makeListView();
    listView->setItemDelegate(md);

    connect(projectSettings, &ProjectSettings::fileNameChanged, [this](const QString& fileName) {
        action->setText(fileName);
    });

    action->setText(projectSettings->getFileName());
    action->setCheckable(true);

    connect(plot, &QAbstractSlider::sliderPressed, mediaPanel, &MediaPanel::pause);
    connect(plot, &QAbstractSlider::sliderReleased, mediaPanel, &MediaPanel::resume);

    viewportUniformBuffer.append({});
    viewport = new Viewport(this);

    editorCamera->action->setText("Default camera");
    editorCamera->action->setIcon(QPixmap(":/dialogs/film camera"));

    cameras->addSeparator();
}

Session::~Session()
{
    delete action;
    delete simulation;
    delete editorCamera;
    delete cameras;
    delete treeView;
    delete projectSettings;
    delete viewport;
    delete mediaPanel;
    delete chart;
    delete plot;
    delete renderSettings;
    delete nd;
    delete vd;
    delete td;
    delete md;
}

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

void Session::fromJson(const QJsonDocument &json)
{
    const QJsonObject project = json.object();

    const QJsonArray materials_ = project["Materials"].toArray();
    qobject_cast<MaterialListModel*>(listView->model())->read(materials_);

    const QJsonObject objects = project["Objects"].toObject();
    simulation->getModel()->read(objects);

    const QJsonObject projectSettings_ = project["Project Settings"].toObject();
    projectSettings->read(projectSettings_);

    const QJsonObject viewport_ = project["Viewport"].toObject();
    viewport->read(viewport_);

    const QJsonObject camera_ = project["Editor Camera"].toObject();
    editorCamera->read(camera_);

    const QJsonArray origin_ = project["Origin"].toArray();
    origin = QVector3D(origin_[0].toDouble(), origin_[1].toDouble(), origin_[2].toDouble());

    const QJsonObject renderSettings_ = project["Render settings"].toObject();
    renderSettings->read(renderSettings_);

    previewRange = project["Preview range"].toBool();
    playMode = static_cast<PlayMode>(project["Play mode"].toInt());

    plot->updateSimulation();
}

QJsonDocument Session::toJson() const
{
    QJsonObject project;

    QJsonObject projectSettings_;
    projectSettings->write(projectSettings_);
    project["Project Settings"] = projectSettings_;

    QJsonObject viewport_;
    viewport->write(viewport_);
    project["Viewport"] = viewport_;

    QJsonObject camera_;
    editorCamera->write(camera_);
    project["Editor Camera"] = camera_;

    QJsonArray origin_({origin[0], origin[1], origin[2]});
    project["Origin"] = origin_;

    QJsonArray materials_;
    qobject_cast<MaterialListModel*>(listView->model())->write(materials_);
    project["Materials"] = materials_;

    QJsonObject objects_;
    simulation->getModel()->write(objects_);
    project["Objects"] = objects_;

    QJsonObject renderSettings_;
    renderSettings->write(renderSettings_);
    project["Render settings"] = renderSettings_;

    project["Preview range"] = previewRange;
    project["Play mode"] = playMode;

    return QJsonDocument(project);
}

void Session::reallocateBuffers()
{
    cameraBuffer.forceReallocate();
    atomBuffer.forceReallocate();
    Material::getBuffer().forceReallocate();
}

void Session::changeCamera(Camera* camera)
{
    currentCamera = camera ? camera : editorCamera;
    cameraUniformBuffer = currentCamera->id;
    mediaPanel->changeCamera(currentCamera);
}

bool Session::openProject(const QString &path)
{
    if (projectSettings->getOpenFileName(path))
    {
        fromJson(projectSettings->readSaveFile());
        return true;
    }

    return false;
}

void Session::saveProject() const
{
    if (projectSettings->getSaveFileName())
        projectSettings->writeSaveFile(toJson());
}

void Session::saveProjectAs() const
{
    if (projectSettings->getNewSaveFileName())
        projectSettings->writeSaveFile(toJson());
}

void Session::setFPS(int fps)
{
    mediaPanel->setFPS(fps);
    projectSettings->setFPS(fps);
    renderSettings->updateFrames();
}

void Session::setDocumentTime(int time)
{
    simulation->readEntry(time, reinterpret_cast<char*>(atomBuffer.data()), sizeof(VizBallInstance), offsetof(VizBallInstance, position));

    mediaPanel->setDocumentTime(time);
    projectSettings->setDocumentTime(time);
    plot->setValue(time, false);
    renderSettings->setDocumentTime(time);

    SplineInterpolator::setFrame(time);

    emit documentTimeChanged(time);
}

void Session::setMinimumTime(int time)
{
    mediaPanel->setMinimumTime(time);
    projectSettings->setMinimumTime(time);
    renderSettings->setMinimumTime(time);
}

void Session::setMaximumTime(int time)
{
    mediaPanel->setMaximumTime(time);
    projectSettings->setMaximumTime(time);
    renderSettings->setMaximumTime(time);
}

void Session::setPreviewMinTime(int time)
{
    mediaPanel->setPreviewMinTime(time);
    projectSettings->setPreviewMinTime(time);
    plot->setSoftMinimum(time);
    renderSettings->setPreviewMinTime(time);
}

void Session::setPreviewMaxTime(int time)
{
    mediaPanel->setPreviewMaxTime(time);
    projectSettings->setPreviewMaxTime(time);
    plot->setSoftMaximum(time);
    renderSettings->setPreviewMaxTime(time);
}

void Session::setLastFrame(int time)
{
    bool expandTime = projectSettings->getMaximumTime() == lastFrame;
    bool expandPreviewTime = projectSettings->getPreviewMaxTime() == lastFrame;

    lastFrame = time;

    mediaPanel->setLastFrame(lastFrame);
    projectSettings->setLastFrame(lastFrame);
    plot->setMaximum(lastFrame);
    renderSettings->setLastFrame(lastFrame);

    if (expandTime)
        setMaximumTime(lastFrame);

    if (expandPreviewTime)
        setPreviewMaxTime(lastFrame);
}

void Session::setOrigin(const QModelIndex& index, bool selected)
{
    QVector3D origin;
    int count = 0;

    const auto model = simulation->getModel();

    std::function<void(const QModelIndex&)> dfs = [&](const QModelIndex& root) {
        for (int r = 0; r < model->rowCount(root); r++)
            dfs(model->index(r, 0, root));

        auto item = dynamic_cast<AtomItem*>(reinterpret_cast<TreeItem*>(root.internalPointer()));

        if (!item)
            return;

        const auto& atom = atomBuffer[item->getId()];

        if (selected && !atom.flags.testFlag(Selected))
            return;

        origin += atom.position;
        count++;
    };

    dfs(index);

    if (count > 0)
        this->origin = origin / count;
}
