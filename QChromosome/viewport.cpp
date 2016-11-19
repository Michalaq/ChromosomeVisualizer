#include "viewport.h"
#include "ui_viewport.h"

static const float DISPLAYED_TO_INTERNAL_FOG_DENSITY = 0.001f;

Viewport::Viewport(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Viewport)
{
    ui->setupUi(this);
    ui->pushButton->setAttribute(Qt::WA_TransparentForMouseEvents);

    ui->widget->setValue(QColor(Qt::black));
    ui->widget_2->setValue(QColor(Qt::black));
    ui->widget_3->setValue(QColor(Qt::black));
    ui->widget_4->setValue(QColor(Qt::white));

    // safe frames
    connect(ui->checkBox, &QCheckBox::toggled, [this] (bool czy) {
        blind_->setVisible(czy);
    });

    // opacity
    connect(ui->doubleSpinBox, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [this](double value) {
        blind_->setOpacity(value / 100);
    });

    // border color
    connect(ui->widget, &Picker::valueChanged, [this] (QColor color) {
        blind_->setColor(color);
    });

    // editor axis position
    connect(ui->comboBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [this](int index) {
        axis_->setPosition(index);
    });

    // editor axis scale
    connect(ui->doubleSpinBox_2, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [this](double value) {
        axis_->setScale(value / 100);
    });

    // editor axis text
    connect(ui->checkBox_2, &QCheckBox::toggled, [this] (bool czy) {
        axis_->setTextVisible(czy);
    });

    // background color
    connect(ui->widget_2, &Picker::valueChanged, [this] (QColor color) {
        vizWidget_->setBackgroundColor(color);
    });

    // fog density
    connect(ui->doubleSpinBox_3, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [this](double value) {
        vizWidget_->setFogDensity(value * DISPLAYED_TO_INTERNAL_FOG_DENSITY);
    });

    // fog contribution
    connect(ui->doubleSpinBox_4, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [this](double value) {
        vizWidget_->setFogContribution(value / 100);
    });

    // labels background
    connect(ui->widget_3, &Picker::valueChanged, [this] (QColor color) {
        vizWidget_->setLabelBackgroundColor(color);
    });

    // labels text
    connect(ui->widget_4, &Picker::valueChanged, [this] (QColor color) {
        vizWidget_->setLabelTextColor(color);
    });
}

Viewport::~Viewport()
{
    delete ui;
}

void Viewport::setVizWidget(VizWidget *vizWidget)
{
    vizWidget_ = vizWidget;
}

void Viewport::setBlind(Blind *blind)
{
    blind_ = blind;
}

void Viewport::setAxis(Axis *axis)
{
    axis_ = axis;
}
