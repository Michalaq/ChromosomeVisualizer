#include "viewport.h"
#include "ui_viewport.h"

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

    // safe frames

    // opacity

    // border color
    /*connect(ui->widget, &Picker::valueChanged, [this] (QColor color) {
        vizWidget_->selectedSpheresObject().setColor(color);
    });*/

    // editor axis position

    // editor axis scale

    // editor axis text

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
