#include "session.h"

Session::Session() :
    QObject(),
    action(new QAction),
    simulation(new Simulation(this)),
    editorCamera(new Camera(this)),
    currentCamera(editorCamera),
    treeView(new TreeView),
    projectSettings(new ProjectSettings(this)),
    viewport(new Viewport),
    mediaPanel(new MediaPanel(this)),
    nd(new NameDelegate),
    vd(new VisibilityDelegate),
    td(new TagsDelegate),
    md(new MaterialDelegate),
    lastFrame(0)
{
    treeView->setMouseTracking(true);
    treeView->setFocusPolicy(Qt::NoFocus);
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

    listView->setFocusPolicy(Qt::NoFocus);
    listView->setItemDelegate(md);

    connect(projectSettings, &ProjectSettings::fileNameChanged, [this](const QString& fileName) {
        action->setText(fileName);
    });

    action->setText(projectSettings->getFileName());
}

Session::~Session()
{
    delete action;
    delete simulation;
    delete editorCamera;
    delete treeView;
    delete projectSettings;
    delete viewport;
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

    const QJsonObject projectSettings_ = project["Project Settings"].toObject();
    projectSettings->read(projectSettings_);

    const QJsonObject viewport_ = project["Viewport"].toObject();
    viewport->read(viewport_);

    const QJsonObject camera_ = project["Editor Camera"].toObject();
    editorCamera->read(camera_);

    const QJsonArray materials_ = project["Materials"].toArray();
    qobject_cast<MaterialListModel*>(listView->model())->read(materials_);

    const QJsonObject objects = project["Objects"].toObject();
    simulation->getModel()->read(objects);

    simulation->getFrame(projectSettings->getMaximumTime());
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

    QJsonArray materials_;
    qobject_cast<MaterialListModel*>(listView->model())->write(materials_);
    project["Materials"] = materials_;

    QJsonObject objects_;
    simulation->getModel()->write(objects_);
    project["Objects"] = objects_;

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
    if (!camera)
        camera = editorCamera;

    currentCamera->blockSignals(true);
    currentCamera = camera;
    currentCamera->blockSignals(false);

    mediaPanel->changeCamera(currentCamera);

    cameraUniformBuffer = &currentCamera->cameraUniformBuffer;
}

bool Session::openProject()
{
    if (projectSettings->getOpenFileName())
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
}

void Session::setDocumentTime(int time)
{
    const auto frame = simulation->getFrame(time);
    const auto& atoms = frame->atoms;

    Q_ASSERT(atoms.size() == atomBuffer.size());

    for (int i = 0; i < atoms.size(); i++)
    {
        const auto& atom = atoms[i];
        atomBuffer[i].position = QVector3D(atom.x, atom.y, atom.z);
    }

    if (lastFrame < simulation->getLastFrame())
        setLastFrame(simulation->getLastFrame());

    mediaPanel->setDocumentTime(time);
    projectSettings->setDocumentTime(time);

    emit documentTimeChanged(time);
}

void Session::setMinimumTime(int time)
{
    mediaPanel->setMinimumTime(time);
    projectSettings->setMinimumTime(time);
}

void Session::setMaximumTime(int time)
{
    mediaPanel->setMaximumTime(time);
    projectSettings->setMaximumTime(time);
}

void Session::setPreviewMinTime(int time)
{
    mediaPanel->setPreviewMinTime(time);
    projectSettings->setPreviewMinTime(time);
}

void Session::setPreviewMaxTime(int time)
{
    mediaPanel->setPreviewMaxTime(time);
    projectSettings->setPreviewMaxTime(time);
}

void Session::setLastFrame(int time)
{
    bool expandTime = projectSettings->getMaximumTime() == lastFrame;
    bool expandPreviewTime = projectSettings->getPreviewMaxTime() == lastFrame;

    lastFrame = time;

    mediaPanel->setLastFrame(lastFrame);
    projectSettings->setLastFrame(lastFrame);

    if (expandTime)
        setMaximumTime(lastFrame);

    if (expandPreviewTime)
        setPreviewMaxTime(lastFrame);
}
