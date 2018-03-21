#include "session.h"

Session::Session() :
    PS_FPS(30)
{
    simulation = new Simulation(this);
    selectionModel = new QItemSelectionModel(simulation->getModel());
}

Session::~Session()
{
    delete simulation;
    delete selectionModel;
}

#include <cassert>

void Session::setFrame(std::shared_ptr<Frame> frame)
{
    assert(frame->atoms.size() == abuffer.size());

    auto& atoms = frame->atoms;

    for (int i = 0; i < atoms.size(); i++)
    {
        auto& atom = atoms[i];
        abuffer[i].position = QVector3D(atom.x, atom.y, atom.z);
    }

    amodified = true;
}

// ProjectSettings;
int Session::PS_getFPS() const
{
    return PS_FPS;
}

void Session::PS_setFPS(int n)
{
    PS_FPS = n;
}

int Session::PS_getDocumentTime() const
{
    return PS_DocumentTime;
}

void Session::PS_setDocumentTime(int n)
{
    PS_DocumentTime = n;
}

int Session::PS_getMinimumTime() const
{
    return PS_MinimumTime;
}

void Session::PS_setMinimumTime(int n)
{
    PS_MinimumTime = n;
}

int Session::PS_getMaximumTime() const
{
    return PS_MaximumTime;
}

void Session::PS_setMaximumTime(int n)
{
    PS_MaximumTime = n;
}

int Session::PS_getPreviewMinTime() const
{
    return PS_PreviewMinTime;
}

void Session::PS_setPreviewMinTime(int n)
{
    PS_PreviewMinTime = n;
}

int Session::PS_getPreviewMaxTime() const
{
    return PS_PreviewMaxTime;
}

void Session::PS_setPreviewMaxTime(int n)
{
    PS_PreviewMaxTime = n;
}

// Info
const QString& Session::I_getAuthor() const
{
    return I_Author;
}

void Session::I_setAuthor(const QString& s)
{
    I_Author = s;
}

const QString& Session::I_getInfo() const
{
    return I_Info;
}

void Session::I_setInfo(const QString& s)
{
    I_Info = s;
}

const QString& Session::I_getFileFormat() const
{
    return I_FileFormat;
}

void Session::I_setFileFormat(const QString& s)
{
    I_FileFormat = s;
}

const QString& Session::I_getFileVersion() const
{
    return I_FileVersion;
}

void Session::I_setFileVersion(const QString& s)
{
    I_FileVersion = s;
}

#include <QJsonObject>

void Session::PS_read(const QJsonObject &json)
{
    const QJsonObject projectSettings = json["Project Settings"].toObject();
    PS_FPS = projectSettings["FPS"].toInt();
    PS_DocumentTime = projectSettings["Document Time"].toInt();
    PS_MinimumTime = projectSettings["Minimum Time"].toInt();
    PS_MaximumTime = projectSettings["Maximum Time"].toInt();
    PS_PreviewMinTime = projectSettings["Preview Min. Time"].toInt();
    PS_PreviewMaxTime = projectSettings["Preview Max. Time"].toInt();

    const QJsonObject info = json["Info"].toObject();
    I_Author = info["Author"].toString();
    I_Info = info["Info"].toString();
    I_FileFormat = info["File Format"].toString();
    I_FileVersion = info["File Version"].toString();
}

void Session::PS_write(QJsonObject &json) const
{
    QJsonObject projectSettings;
    projectSettings["FPS"] = PS_FPS;
    projectSettings["Document Time"] = PS_DocumentTime;
    projectSettings["Minimum Time"] = PS_MinimumTime;
    projectSettings["Maximum Time"] = PS_MaximumTime;
    projectSettings["Preview Min. Time"] = PS_PreviewMinTime;
    projectSettings["Preview Max. Time"] = PS_PreviewMaxTime;
    json["Project Settings"] = projectSettings;

    QJsonObject info;
    info["Author"] = I_Author;
    info["Info"] = I_Info;
    info["File Format"] = I_FileFormat;
    info["File Version"] = I_FileVersion;
    json["Info"] = info;
}
