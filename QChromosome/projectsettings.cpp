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

    ui->spinBox->setValue(session->PS_FPS);
    ui->spinBox_5->setValue(session->PS_DocumentTime);
    ui->spinBox_3->setValue(session->PS_MinimumTime);
    ui->spinBox_6->setValue(session->PS_MaximumTime);
    ui->spinBox_4->setValue(session->PS_PreviewMinTime);
    ui->spinBox_7->setValue(session->PS_PreviewMaxTime);

    ui->lineEdit_3->setText(session->I_Author);
    ui->textEdit->setPlainText(session->I_Info);
    ui->lineEdit_4->setText(session->I_FileFormat);
    ui->lineEdit_5->setText(session->I_FileVersion);
}
