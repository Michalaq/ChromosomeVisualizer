#include "session.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ui_projectsettings.h"

Session::Session(MainWindow *parent) :
    window(parent),
    PS_FPS(30),
    PS_DocumentTime(0),
    PS_PreviewMinTime(0),
    PS_PreviewMaxTime(0)
{
    simulation = new Simulation(this);
    selectionModel = new QItemSelectionModel(simulation->getModel());

    I_setFilePath(QDir(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)).filePath("Untitled"));
}

Session::~Session()
{
    delete simulation;
    delete selectionModel;
}

// ProjectSettings;
int Session::PS_getFPS() const
{
    return PS_FPS;
}

void Session::PS_setFPS(int n)
{
    PS_FPS = n;

    window->timer.setInterval(1000 / n);
}

int Session::PS_getDocumentTime() const
{
    return PS_DocumentTime;
}

#include <cassert>

void Session::PS_setDocumentTime(int n)
{
    PS_DocumentTime = n;

    window->currentFrame = n;
    window->ui->horizontalSlider->setValue(n);
    window->ui->spinBox->setValue(n);
    window->ui->scene->update();
    window->ui->plot->setValue(n);
    SplineInterpolator::setFrame(n);
    window->ui->page->ui->spinBox_5->setValue(n, false);

    auto& atoms = simulation->getFrame(n)->atoms;

    assert(atoms.size() == abuffer.size());

    for (int i = 0; i < atoms.size(); i++)
    {
        auto& atom = atoms[i];
        abuffer[i].position = QVector3D(atom.x, atom.y, atom.z);
    }

    amodified = true;
}

int Session::PS_getMinimumTime() const
{
    return PS_MinimumTime;
}

void Session::PS_setMinimumTime(int n)
{
    PS_MinimumTime = n;

    window->ui->horizontalSlider_2->setMinimum(n);
    window->ui->spinBox_2->setValue(n, false);
    window->ui->spinBox_3->setMinimum(n);
    window->ui->page->ui->spinBox_6->setMinimum(n);
    window->ui->page->ui->spinBox_4->setMinimum(n);
    window->ui->page->ui->spinBox_3->setValue(n, false);
}

int Session::PS_getMaximumTime() const
{
    return PS_MaximumTime;
}

void Session::PS_setMaximumTime(int n)
{
    PS_MaximumTime = n;

    window->ui->horizontalSlider_2->setMaximum(n);
    window->ui->spinBox_3->setValue(n, false);
    window->ui->spinBox_2->setMaximum(n);
    window->ui->page->ui->spinBox_3->setMaximum(n);
    window->ui->page->ui->spinBox_7->setMaximum(n);
    window->ui->page->ui->spinBox_6->setValue(n, false);
}

int Session::PS_getPreviewMinTime() const
{
    return PS_PreviewMinTime;
}

void Session::PS_setPreviewMinTime(int n)
{
    PS_PreviewMinTime = n;

    window->ui->horizontalSlider->setSoftMinimum(n);
    window->ui->horizontalSlider_2->setLowerBound(n, false);
    window->ui->plot->setSoftMinimum(n);
    window->ui->page->ui->spinBox_7->setMinimum(n);
    window->ui->page->ui->spinBox_4->setValue(n, false);

    window->softMinimum = n;
}

int Session::PS_getPreviewMaxTime() const
{
    return PS_PreviewMaxTime;
}

void Session::PS_setPreviewMaxTime(int n)
{
    PS_PreviewMaxTime = n;

    window->ui->horizontalSlider->setSoftMaximum(n);
    window->ui->horizontalSlider_2->setUpperBound(n, false);
    window->ui->plot->setSoftMaximum(n);
    window->ui->page->ui->spinBox_4->setMaximum(n);
    window->ui->page->ui->spinBox_7->setValue(n, false);

    window->softMaximum = n;
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

const QString& Session::I_getFilePath() const
{
    return I_FilePath;
}

void Session::I_setFilePath(const QString& s)
{
    I_FilePath = s;

    window->ui->page->ui->lineEdit_6->setText(s);
    window->setWindowTitle(QString("QChromosome 4D Studio - [%1]").arg(QFileInfo(s).fileName()));
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

    PS_setDocumentTime(PS_DocumentTime);
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