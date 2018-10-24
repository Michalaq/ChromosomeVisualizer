#include "importdialog.h"
#include "ui_importdialog.h"

#include <QFileInfo>

ImportDialog::ImportDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ImportDialog),
    unlimited(true)
{
    ui->setupUi(this);

    setFixedSize(width(), minimumHeight());

    connect(ui->spinBox, QOverload<int>::of(&SpinBox::valueChanged), [this](int value) {
        ui->spinBox_2->setMinimum(value - 1, false);
        if (unlimited) ui->spinBox_2->setValue(value - 1, false);
    });

    connect(ui->spinBox_2, QOverload<int>::of(&SpinBox::valueChanged), [this](int value) {
        unlimited = value == ui->spinBox_2->minimum();
        ui->spinBox->setMaximum(unlimited ? INT_MAX : value, false);
    });
}

ImportDialog::~ImportDialog()
{
    delete ui;
}

int ImportDialog::first() const
{
    return ui->spinBox->value();
}

int ImportDialog::last() const
{
    return unlimited ? INT_MAX : ui->spinBox_2->value();
}

int ImportDialog::stride() const
{
    return ui->spinBox_3->value();
}

bool ImportDialog::loadInBackground() const
{
    return ui->radioButton->isChecked();
}
