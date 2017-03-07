#include "projectsettings.h"
#include "ui_projectsettings.h"

#include <QSettings>

ProjectSettings::ProjectSettings(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Form)
{
    ui->setupUi(this);

    connect(ui->lineEdit, &QLineEdit::editingFinished, [this]() {
        QSettings().setValue("povraypath", ui->lineEdit->text());
    });

    connect(ui->lineEdit_2, &QLineEdit::editingFinished, [this]() {
        QSettings().setValue("locallib", ui->lineEdit_2->text());
    });

    QSettings settings;
    ui->lineEdit->setText(settings.value("povraypath", "/usr/local/share/povray-3.7").toString());
    ui->lineEdit_2->setText(settings.value("locallib", "/home").toString());
}

ProjectSettings::~ProjectSettings()
{
    delete ui;
}
