#include "targadialog.h"
#include "ui_targadialog.h"

TARGADialog::TARGADialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TARGADialog)
{
    ui->setupUi(this);
    setFixedSize(width(), minimumHeight());

    connect(this, &QDialog::finished, [this](int result) {
        if (result)
            index = ui->comboBox->currentIndex();
        else
            ui->comboBox->setCurrentIndex(index);
    });

    ui->comboBox->setCurrentIndex(index = 0);
}

TARGADialog::~TARGADialog()
{
    delete ui;
}

int TARGADialog::getCompression() const
{
    return index;
}

#include <QJsonObject>

void TARGADialog::read(const QJsonObject& json)
{
    if (json.contains("TARGA Settings"))
    {
        QJsonObject settings = json["TARGA Settings"].toObject();
        ui->comboBox->setCurrentText(settings["Compression"].toString());
        index = ui->comboBox->currentIndex();
    }
}

void TARGADialog::write(QJsonObject& json) const
{
    QJsonObject settings;
    settings["Compression"] = ui->comboBox->currentText();
    json["TARGA Settings"] = settings;
}
