#include "jpegdialog.h"
#include "ui_jpegdialog.h"

JPEGDialog::JPEGDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::JPEGDialog)
{
    ui->setupUi(this);
    setFixedSize(width(), minimumHeight());

    connect(this, &QDialog::finished, [this](int result) {
        if (result)
            quality = ui->spinBox->value();
        else
            setQuality(quality);
    });

    connect(ui->spinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &JPEGDialog::setQuality);
    connect(ui->horizontalSlider, &QSlider::valueChanged, this, &JPEGDialog::setQuality);

    setQuality(quality = 85);
}

JPEGDialog::~JPEGDialog()
{
    delete ui;
}

void JPEGDialog::setQuality(int q)
{
    ui->spinBox->setValue(q, false);
    ui->horizontalSlider->setValue(q);
}

int JPEGDialog::getQuality() const
{
    return quality;
}

#include <QJsonObject>

void JPEGDialog::read(const QJsonObject& json)
{
    if (json.contains("JPEG Settings"))
    {
        QJsonObject settings = json["JPEG Settings"].toObject();
        setQuality(settings["JPEG Quality"].toInt());
    }
}

void JPEGDialog::write(QJsonObject& json) const
{
    QJsonObject settings;
    settings["JPEG Quality"] = ui->spinBox->value();
    json["JPEG Settings"] = settings;
}
