#include "projectsettings.h"
#include "ui_projectsettings.h"

#include <QSettings>
#include <QFileDialog>

ProjectSettings::ProjectSettings(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Form)
{
    ui->setupUi(this);

    connect(ui->toolButton, &QToolButton::clicked, [this] {
        QString path = QFileDialog::getExistingDirectory(this, tr("Open Directory"), ui->lineEdit->text(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

        if (!path.isEmpty())
        {
            ui->lineEdit->setText(path);

            QSettings settings;
            settings.setValue("povraypath", path);
        }
    });

    connect(ui->lineEdit, &QLineEdit::textChanged, [this](const QString& path) {
        QSettings settings;
        settings.setValue("povraypath", path);
    });

    QSettings settings;
    ui->lineEdit->setText(settings.value("povraypath", "/usr/local/share/povray-3.7").toString());
}

ProjectSettings::~ProjectSettings()
{
    delete ui;
}
