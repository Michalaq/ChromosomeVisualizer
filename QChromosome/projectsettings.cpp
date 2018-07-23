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

void ProjectSettings::setSession(Session *s)
{
    session = s;

    ui->spinBox->setValue(session->FPS, true);
    ui->spinBox_5->setValue(session->documentTime, true);
    ui->spinBox_3->setValue(session->minimumTime, true);
    ui->spinBox_6->setValue(session->maximumTime, true);
    ui->spinBox_4->setValue(session->previewMinTime, true);
    ui->spinBox_7->setValue(session->previewMaxTime, true);

    ui->lineEdit_3->setText(session->author, true);
    ui->textEdit->setText(session->info, true);
    ui->lineEdit_4->setText(session->fileFormat, true);
    ui->lineEdit_5->setText(session->fileVersion, true);
}
