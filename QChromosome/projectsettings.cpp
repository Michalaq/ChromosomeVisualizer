#include "projectsettings.h"
#include "ui_projectsettings.h"

ProjectSettings::ProjectSettings(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Form)
{
    ui->setupUi(this);
}

ProjectSettings::~ProjectSettings()
{
    delete ui;
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
    info["File Version"] = ui->lineEdit_5->text();
    json["Info"] = info;
}
