#include "projectsettings.h"
#include "ui_projectsettings.h"

#include <QSettings>

ProjectSettings::ProjectSettings(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Form)
{
    ui->setupUi(this);

    connect(ui->lineEdit, &QLineEdit::editingFinished, [this]() {
        QSettings settings;
        settings.setValue("povraypath", ui->lineEdit->text());
    });

    QSettings settings;
    ui->lineEdit->setText(settings.value("povraypath", "/usr/local/share/povray-3.7").toString());
}

ProjectSettings::~ProjectSettings()
{
    delete ui;
}
