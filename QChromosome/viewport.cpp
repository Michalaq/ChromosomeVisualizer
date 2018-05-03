#include "viewport.h"
#include "ui_viewport.h"

viewport_data_t Viewport::buffer;
bool Viewport::modified = false;

static const float DISPLAYED_TO_INTERNAL_FOG_DENSITY = 0.001f;

Viewport::Viewport(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Viewport)
{
    ui->setupUi(this);

    ui->widget->setValue(QColor(Qt::black));
    ui->widget_2->setValue(QColor(Qt::black));
    ui->widget_3->setValue(QColor(Qt::black));
    ui->widget_4->setValue(QColor(Qt::white));
    ui->doubleSpinBox_3->setValue(10);
    ui->doubleSpinBox_4->setValue(80);

    buffer.ucBackgroundColor = ui->widget_2->value().rgb();
    buffer.ufFogDensity = ui->doubleSpinBox_3->value() * DISPLAYED_TO_INTERNAL_FOG_DENSITY;
    buffer.ufFogContribution = ui->doubleSpinBox_4->value() / 100;
    modified = true;

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
        buffer.ucBackgroundColor = val.rgb();
        modified = true;
        emit viewportChanged();
    });

    // fog density
    connect(ui->doubleSpinBox_3, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [this](double val) {
        buffer.ufFogDensity = val * DISPLAYED_TO_INTERNAL_FOG_DENSITY;
        modified = true;
        emit viewportChanged();
    });

    // fog contribution
    connect(ui->doubleSpinBox_4, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [this](double val) {
        buffer.ufFogContribution = val / 100;
        modified = true;
        emit viewportChanged();
    });

    // labels background
    connect(ui->widget_3, &Picker::valueChanged, [this] { emit viewportChanged(); });

    // labels text
    connect(ui->widget_4, &Picker::valueChanged, [this] { emit viewportChanged(); });
}

Viewport::~Viewport()
{
    delete ui;
}

QColor Viewport::getBackgroundColor() const
{
    return ui->widget_2->value();
}

QColor Viewport::getLabelTextColor() const
{
    return ui->widget_4->value();
}

QColor Viewport::getLabelBackgroundColor() const
{
    return ui->widget_3->value();
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

double Viewport::getFogDensity() const
{
    return ui->doubleSpinBox_3->value() * DISPLAYED_TO_INTERNAL_FOG_DENSITY;
}

double Viewport::getFogContribution() const
{
    return ui->doubleSpinBox_4->value() / 100;
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

const viewport_data_t& Viewport::getBuffer()
{
    return buffer;
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

    const QJsonObject atomLabels = json["Atom labels"].toObject();
    ui->widget_3->setValue(atomLabels["Background color"].toString());
    ui->widget_4->setValue(atomLabels["Text color"].toString());
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

    QJsonObject atomLabels;
    atomLabels["Background color"] = ui->widget_3->value().name();
    atomLabels["Text color"] = ui->widget_4->value().name();
    json["Atom labels"] = atomLabels;
}
