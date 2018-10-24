#include "projectsettings.h"
#include "ui_projectsettings.h"
#include <QDir>
#include <QStandardPaths>
#include "session.h"

const char * ProjectSettings::suffix = "qcs";
const char * ProjectSettings::version = "1.01";

int ProjectSettings::count = 0;

ProjectSettings::ProjectSettings(Session* s, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ProjectSettings),
    session(s)
{
    ui->setupUi(this);

    const QString path = QDir(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)).filePath(QString("Untitled %1").arg(++count));

    filePath.setFile(path);
    ui->lineEdit_6->setText(path);

    connect(ui->lineEdit_6, &FileEdit::textChanged, [this]() {
        emit fileNameChanged(getFileName());
    });

    connect(ui->spinBox, QOverload<int>::of(&QSpinBox::valueChanged), session, &Session::setFPS);
    connect(ui->spinBox_5, QOverload<int>::of(&QSpinBox::valueChanged), session, &Session::setDocumentTime);
    connect(ui->spinBox_3, QOverload<int>::of(&QSpinBox::valueChanged), session, &Session::setMinimumTime);
    connect(ui->spinBox_6, QOverload<int>::of(&QSpinBox::valueChanged), session, &Session::setMaximumTime);
    connect(ui->spinBox_4, QOverload<int>::of(&QSpinBox::valueChanged), session, &Session::setPreviewMinTime);
    connect(ui->spinBox_7, QOverload<int>::of(&QSpinBox::valueChanged), session, &Session::setPreviewMaxTime);
}

ProjectSettings::~ProjectSettings()
{
    delete ui;
}

int ProjectSettings::getFPS() const
{
    return ui->spinBox->value();
}

int ProjectSettings::getDocumentTime() const
{
    return ui->spinBox_5->value();
}

int ProjectSettings::getMinimumTime() const
{
    return ui->spinBox_3->value();
}

int ProjectSettings::getMaximumTime() const
{
    return ui->spinBox_6->value();
}

int ProjectSettings::getPreviewMinTime() const
{
    return ui->spinBox_4->value();
}

int ProjectSettings::getPreviewMaxTime() const
{
    return ui->spinBox_7->value();
}

void ProjectSettings::setFPS(int fps)
{
    ui->spinBox->setValue(fps, false);
}

void ProjectSettings::setDocumentTime(int time)
{
    ui->spinBox_5->setValue(time, false);
}

void ProjectSettings::setMinimumTime(int time)
{
    ui->spinBox_3->setValue(time, false);
    ui->spinBox_6->setMinimum(time);
    ui->spinBox_4->setMinimum(time);
}

void ProjectSettings::setMaximumTime(int time)
{
    ui->spinBox_6->setValue(time, false);
    ui->spinBox_3->setMaximum(time);
    ui->spinBox_7->setMaximum(time);
}

void ProjectSettings::setPreviewMinTime(int time)
{
    ui->spinBox_4->setValue(time, false);
    ui->spinBox_7->setMinimum(time);
}

void ProjectSettings::setPreviewMaxTime(int time)
{
    ui->spinBox_7->setValue(time, false);
    ui->spinBox_4->setMaximum(time);
}

void ProjectSettings::setLastFrame(int time)
{
    ui->spinBox_5->setMaximum(time);
    ui->spinBox_6->setMaximum(time);
}

QString ProjectSettings::getFileName() const
{
    return filePath.fileName();
}

#include <QFileDialog>

bool ProjectSettings::getOpenFileName()
{
    QString path = QFileDialog::getOpenFileName(0, "Open...", QStandardPaths::writableLocation(QStandardPaths::HomeLocation), QString("QChromosome 4D Project File (*.%1)").arg(suffix));

    if (!path.isEmpty())
    {
        filePath.setFile(path);
        ui->lineEdit_6->setText(path);

        return true;
    }

    return false;
}

bool ProjectSettings::getSaveFileName()
{
    if (filePath.exists())
        return true;
    else
        return getNewSaveFileName();
}

bool ProjectSettings::getNewSaveFileName()
{
    QString path = QFileDialog::getSaveFileName(0, "", filePath.exists() ? filePath.path() : QStandardPaths::writableLocation(QStandardPaths::HomeLocation), QString("QChromosome 4D Project File (*.%1)").arg(suffix));

    if (!path.isEmpty())
    {
        if (QFileInfo(path).suffix() != suffix)
            path += QString(".") + suffix;

        filePath.setFile(path);
        ui->lineEdit_6->setText(path);

        return true;
    }

    return false;
}

#include <QJsonDocument>

QJsonDocument ProjectSettings::readSaveFile() const
{
    QFile file(filePath.filePath());
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    const QJsonDocument project = QJsonDocument::fromJson(file.readAll());
    file.close();

    count--;

    return project;
}

void ProjectSettings::writeSaveFile(const QJsonDocument& project)
{
    ui->lineEdit_4->setText(QString("QChromosome 4D File (*.%1)").arg(suffix));
    ui->lineEdit_5->setText(version);

    QFile file(filePath.filePath());
    file.open(QIODevice::WriteOnly | QIODevice::Text);
    file.write(project.toJson());
    file.close();
}

#include <QJsonObject>

void ProjectSettings::read(const QJsonObject &json)
{
    const QJsonObject projectSettings = json["Project Settings"].toObject();
    session->setFPS(projectSettings["FPS"].toInt());
    session->setLastFrame(projectSettings["Maximum Time"].toInt());
    session->setMinimumTime(projectSettings["Minimum Time"].toInt());
    session->setMaximumTime(projectSettings["Maximum Time"].toInt());
    session->setDocumentTime(projectSettings["Document Time"].toInt());
    session->setPreviewMinTime(projectSettings["Preview Min. Time"].toInt());
    session->setPreviewMaxTime(projectSettings["Preview Max. Time"].toInt());

    const QJsonObject info = json["Info"].toObject();
    ui->lineEdit_3->setText(info["Author"].toString());
    ui->textEdit->setPlainText(info["Info"].toString());
    ui->lineEdit_4->setText(info["File Format"].toString());
    ui->lineEdit_5->setText(info["File Version"].toString());
}

void ProjectSettings::write(QJsonObject &json) const
{
    QJsonObject projectSettings;
    projectSettings["FPS"] = ui->spinBox->value();
    projectSettings["Document Time"] = ui->spinBox_5->value();
    projectSettings["Minimum Time"] = ui->spinBox_3->value();
    projectSettings["Maximum Time"] = ui->spinBox_6->value();
    projectSettings["Preview Min. Time"] = ui->spinBox_4->value();
    projectSettings["Preview Max. Time"] = ui->spinBox_7->value();
    json["Project Settings"] = projectSettings;

    QJsonObject info;
    info["Author"] = ui->lineEdit_3->text();
    info["Info"] = ui->textEdit->toPlainText();
    info["File Format"] = ui->lineEdit_4->text();
    info["File Version"] = version;
    json["Info"] = info;
}
