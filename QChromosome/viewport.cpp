#include "viewport.h"
#include "ui_viewport.h"
#include "session.h"

Viewport::Viewport(Session *s, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Viewport),
    session(s)
{
    ui->setupUi(this);

    // safe frames
    connect(ui->checkBox, &QCheckBox::toggled, [this] { emit viewportChanged(); });

    // opacity
    connect(ui->doubleSpinBox, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [this] { emit viewportChanged(); });

    // border color
    connect(ui->widget, &Picker::valueChanged, [this] { emit viewportChanged(); });

    // editor axis position
    connect(ui->comboBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [this] { emit viewportChanged(); });

    // editor axis scale
    connect(ui->doubleSpinBox_2, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [this] { emit viewportChanged(); });

    // editor axis text
    connect(ui->checkBox_2, &QCheckBox::toggled, [this] { emit viewportChanged(); });

    // background color
    connect(ui->widget_2, &Picker::valueChanged, [this](QColor val) {
        session->viewportUniformBuffer[0].ucBackgroundColor = val.rgb();
        emit viewportChanged();
    });

    // environment color
    connect(ui->comboBox_2, &Picker::valueChanged, [this](QColor val) {
        session->viewportUniformBuffer[0].ucEnvironmentColor = val.rgb();
        emit viewportChanged();
    });

    // environment strength
    connect(ui->doubleSpinBox_5, QOverload<double>::of(&DoubleSpinBox::valueChanged), [this](double val) {
        session->viewportUniformBuffer[0].ufEnvironmentStrength = val / 100;
        emit viewportChanged();
    });

    // enable fog
    connect(ui->checkBox_3, &QCheckBox::toggled, [this](bool checked) {
        ui->widget_5->setEnabled(checked);
        session->viewportUniformBuffer[0].ubEnableFog = checked;
        emit viewportChanged();
    });

    // fog color
    connect(ui->comboBox_3, &Picker::valueChanged, [this](QColor val) {
        session->viewportUniformBuffer[0].ucFogColor = val.rgb();
        emit viewportChanged();
    });

    // fog strength
    connect(ui->doubleSpinBox_3, QOverload<double>::of(&DoubleSpinBox::valueChanged), [this](double val) {
        session->viewportUniformBuffer[0].ufFogStrength = val / 100;
        emit viewportChanged();
    });

    // fog distance
    connect(ui->doubleSpinBox_4, QOverload<double>::of(&DoubleSpinBox::valueChanged), [this](double val) {
        session->viewportUniformBuffer[0].ufFogDistance = val;
        emit viewportChanged();
    });

    // border color
    ui->widget->setValue(QColor(Qt::black));
    // background color
    ui->widget_2->setValue(QColor(Qt::black));
    // environment color
    ui->comboBox_2->setValue(QColor(Qt::black));
    // environment strength
    ui->doubleSpinBox_5->setValue(50);
    // enable fog
    ui->checkBox_3->setChecked(true);
    // fog color
    ui->comboBox_3->setValue(QColor(Qt::black));
    // fog strength
    ui->doubleSpinBox_3->setValue(50);
    // fog distance
    ui->doubleSpinBox_4->setValue(200);
}

Viewport::~Viewport()
{
    delete ui;
}

double Viewport::getSFOpacity() const
{
    return ui->doubleSpinBox->value() / 100;
}

QColor Viewport::getSFColor() const
{
    return ui->widget->value();
}

bool Viewport::getSFVisible() const
{
    return ui->checkBox->isChecked();
}

AxisPosition Viewport::getAxisPosition() const
{
    return static_cast<AxisPosition>(ui->comboBox->currentIndex());
}

double Viewport::getAxisScale() const
{
    return ui->doubleSpinBox_2->value() / 100;
}

bool Viewport::getAxisTextVisible() const
{
    return ui->checkBox_2->isChecked();
}

#include <QJsonObject>

void Viewport::read(const QJsonObject &json)
{
    const QJsonObject view = json["View"].toObject();
    ui->checkBox->setChecked(view["Safe frames"].toBool());
    ui->doubleSpinBox->setValue(view["Opacity"].toDouble());
    ui->widget->setValue(view["Border color"].toString());

    const QJsonObject editorAxis = json["Editor axis"].toObject();
    ui->comboBox->setCurrentText(editorAxis["Position"].toString());
    ui->doubleSpinBox_2->setValue(editorAxis["Scale"].toDouble());
    ui->checkBox_2->setChecked(editorAxis["Text"].toBool());

    const QJsonObject background = json["Background"].toObject();
    ui->widget_2->setValue(background["Color"].toString());

    const QJsonObject environment = json["Environment"].toObject();
    ui->doubleSpinBox_3->setValue(environment["Fog density"].toDouble());
    ui->doubleSpinBox_4->setValue(environment["Fog contribution"].toDouble());

    /*const QJsonObject atomLabels = json["Atom labels"].toObject();
    ui->widget_3->setValue(atomLabels["Background color"].toString());
    ui->widget_4->setValue(atomLabels["Text color"].toString());*/
}

void Viewport::write(QJsonObject &json) const
{
    QJsonObject view;
    view["Safe frames"] = ui->checkBox->isChecked();
    view["Opacity"] = ui->doubleSpinBox->value();
    view["Border color"] = ui->widget->value().name();
    json["View"] = view;

    QJsonObject editorAxis;
    editorAxis["Position"] = ui->comboBox->currentText();
    editorAxis["Scale"] = ui->doubleSpinBox_2->value();
    editorAxis["Text"] = ui->checkBox_2->isChecked();
    json["Editor axis"] = editorAxis;

    QJsonObject background;
    background["Color"] = ui->widget_2->value().name();
    json["Background"] = background;

    QJsonObject environment;
    environment["Fog density"] = ui->doubleSpinBox_3->value();
    environment["Fog contribution"] = ui->doubleSpinBox_4->value();
    json["Environment"] = environment;

    /*QJsonObject atomLabels;
    atomLabels["Background color"] = ui->widget_3->value().name();
    atomLabels["Text color"] = ui->widget_4->value().name();
    json["Atom labels"] = atomLabels;*/
}
