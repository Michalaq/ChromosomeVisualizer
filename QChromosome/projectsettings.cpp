#include "projectsettings.h"
#include "ui_projectsettings.h"
#include <QDir>
#include <QStandardPaths>

const char * ProjectSettings::suffix = "qcs";
const char * ProjectSettings::version = "1.01";

ProjectSettings::ProjectSettings(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ProjectSettings)
{
    ui->setupUi(this);

    const QString path = QDir(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)).filePath("Untitled");

    filePath.setFile(path);
    ui->lineEdit_6->setText(path);

    connect(ui->lineEdit_6, &FileEdit::textChanged, [this]() {
        emit fileNameChanged(getFileName());
    });
}

ProjectSettings::~ProjectSettings()
{
    delete ui;
}

int ProjectSettings::getDocumentTime() const
{
    return ui->spinBox_5->value();
}

QString ProjectSettings::getFileName() const
{
    return filePath.fileName();
}

int ProjectSettings::getFPS() const
{
    return ui->spinBox->value();
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
    ui->spinBox->setValue(projectSettings["FPS"].toInt());
    ui->spinBox_5->setValue(projectSettings["Document Time"].toInt());
    ui->spinBox_3->setValue(projectSettings["Minimum Time"].toInt());
    ui->spinBox_6->setValue(projectSettings["Maximum Time"].toInt());
    ui->spinBox_4->setValue(projectSettings["Preview Min. Time"].toInt());
    ui->spinBox_7->setValue(projectSettings["Preview Max. Time"].toInt());

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
