#include "session.h"
#include <QDir>
#include <QStandardPaths>

Session::Session() :
    action(new QAction),
    simulation(new Simulation(this)),
    editorCamera(new Camera(this)),
    treeView(new TreeView),
    FPS(30),
    nd(new NameDelegate),
    vd(new VisibilityDelegate),
    td(new TagsDelegate)
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

    filePath.setFile(QDir(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)).filePath("Untitled"));
}

Session::~Session()
{
    delete action;
    delete simulation;
    delete nd;
    delete vd;
    delete td;
}

#include <cassert>

void Session::setFrame(std::shared_ptr<Frame> frame)
{
    assert(frame->atoms.size() == atomBuffer.size());

    auto& atoms = frame->atoms;

    for (int i = 0; i < atoms.size(); i++)
    {
        auto& atom = atoms[i];
        atomBuffer[i].position = QVector3D(atom.x, atom.y, atom.z);
    }
}

#include <QJsonObject>

void Session::read(const QJsonObject &json)
{
    const QJsonObject projectSettings = json["Project Settings"].toObject();
    FPS = projectSettings["FPS"].toInt();
    documentTime = projectSettings["Document Time"].toInt();
    minimumTime = projectSettings["Minimum Time"].toInt();
    maximumTime = projectSettings["Maximum Time"].toInt();
    previewMinTime = projectSettings["Preview Min. Time"].toInt();
    previewMaxTime = projectSettings["Preview Max. Time"].toInt();

    const QJsonObject projectInfo = json["Info"].toObject();
    author = projectInfo["Author"].toString();
    info = projectInfo["Info"].toString();
    fileFormat = projectInfo["File Format"].toString();
    fileVersion = projectInfo["File Version"].toString();
}

void Session::write(QJsonObject &json) const
{
    QJsonObject projectSettings;
    projectSettings["FPS"] = FPS;
    projectSettings["Document Time"] = documentTime;
    projectSettings["Minimum Time"] = minimumTime;
    projectSettings["Maximum Time"] = maximumTime;
    projectSettings["Preview Min. Time"] = previewMinTime;
    projectSettings["Preview Max. Time"] = previewMaxTime;
    json["Project Settings"] = projectSettings;

    QJsonObject projectInfo;
    projectInfo["Author"] = author;
    projectInfo["Info"] = info;
    projectInfo["File Format"] = fileFormat;
    projectInfo["File Version"] = fileVersion;
    json["Info"] = projectInfo;
}
