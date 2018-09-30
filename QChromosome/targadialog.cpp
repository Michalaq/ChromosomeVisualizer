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
