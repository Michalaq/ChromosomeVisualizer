#include "jpegdialog.h"
#include "ui_jpegdialog.h"

JPEGDialog::JPEGDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::JPEGDialog)
{
    ui->setupUi(this);

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
