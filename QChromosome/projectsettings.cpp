#include "projectsettings.h"
#include "ui_projectsettings.h"

ProjectSettings::ProjectSettings(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Form)
{
    ui->setupUi(this);

    connect(ui->spinBox, QOverload<int>::of(&QSpinBox::valueChanged), [this](int value) {
        session->PS_setFPS(value);
    });

    connect(ui->spinBox_5, QOverload<int>::of(&QSpinBox::valueChanged), [this](int value) {
        session->PS_setDocumentTime(value);
    });

    connect(ui->spinBox_3, QOverload<int>::of(&QSpinBox::valueChanged), [this](int value) {
        session->PS_setMinimumTime(value);
    });

    connect(ui->spinBox_6, QOverload<int>::of(&QSpinBox::valueChanged), [this](int value) {
        session->PS_setMaximumTime(value);
    });

    connect(ui->spinBox_4, QOverload<int>::of(&QSpinBox::valueChanged), [this](int value) {
        session->PS_setPreviewMinTime(value);
    });

    connect(ui->spinBox_7, QOverload<int>::of(&QSpinBox::valueChanged), [this](int value) {
        session->PS_setPreviewMaxTime(value);
    });
}

ProjectSettings::~ProjectSettings()
{
    delete ui;
}

void ProjectSettings::setSession(Session *s)
{
    session = s;

    ui->spinBox->setValue(session->PS_getFPS());
    ui->spinBox_5->setValue(session->PS_getDocumentTime());
    ui->spinBox_3->setValue(session->PS_getMinimumTime());
    ui->spinBox_6->setValue(session->PS_getMaximumTime());
    ui->spinBox_4->setValue(session->PS_getPreviewMinTime());
    ui->spinBox_7->setValue(session->PS_getPreviewMaxTime());

    ui->lineEdit_3->setText(session->I_getAuthor());
    ui->textEdit->setPlainText(session->I_getInfo());
    ui->lineEdit_4->setText(session->I_getFileFormat());
    ui->lineEdit_5->setText(session->I_getFileVersion());
    ui->lineEdit_6->setText(session->I_getFilePath());
}
