#include "session.h"

Session::Session() :
    action(new QAction),
    simulation(new Simulation(this)),
    editorCamera(new Camera(this)),
    currentCamera(editorCamera),
    treeView(new TreeView),
    projectSettings(new ProjectSettings),
    viewport(new Viewport),
    nd(new NameDelegate),
    vd(new VisibilityDelegate),
    td(new TagsDelegate),
    md(new MaterialDelegate)
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
}

Session::~Session()
{
    delete action;
    delete simulation;
    delete nd;
    delete vd;
    delete td;
    delete md;
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
#include <QJsonArray>

void Session::write(QJsonObject& project) const
{
    QJsonObject projectSettings_;
    projectSettings->write(projectSettings_);
    project["Project Settings"] = projectSettings_;

    QJsonObject viewport_;
    viewport->write(viewport_);
    project["Viewport"] = viewport_;

    QJsonObject camera_;
    editorCamera->write(camera_);
    project["Camera"] = camera_;

    QJsonArray materials_;
    qobject_cast<MaterialListModel*>(listView->model())->write(materials_);
    project["Materials"] = materials_;

    QJsonObject objects_;
    simulation->getModel()->write(objects_);
    project["Objects"] = objects_;
}
