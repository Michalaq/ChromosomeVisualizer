#include "DisplayParametersWidget.hpp"
#include "VizWidget.hpp"

DisplayParametersWidget::DisplayParametersWidget(QWidget *parent)
    : QWidget(parent)
    , vizWidget_(nullptr)
{
    initializeControls();
    initializeSignals();
}

void DisplayParametersWidget::setVizWidget(VizWidget * widget)
{
    vizWidget_ = widget;
    fogDensitySlider_->setValue(vizWidget_->fogDensity());
    fogContributionSlider_->setValue(vizWidget_->fogContribution());
}

void DisplayParametersWidget::initializeControls()
{
    auto * label2 = new QLabel("Customize display parameters:");
    backgroundColorButton_ = new QPushButton("Background color");
    fogDensitySlider_ = new QDoubleSpinBox();
    fogDensitySlider_->setRange(0.1, 100.0);
    fogDensitySlider_->setSingleStep(0.1);
    fogContributionSlider_ = new QDoubleSpinBox();
    fogContributionSlider_->setRange(0.0, 1.0);
    fogContributionSlider_->setSingleStep(0.1);
    labelBackgroundColorButton_ = new QPushButton("Label background color");
    labelTextColorButton_ = new QPushButton("Label text color");

    // auto layout = new QFormLayout();
    auto layout = new QGridLayout();

    layout->addWidget(label2, 0, 0, 1, 2);
    layout->addWidget(backgroundColorButton_, 1, 0, 1, 2);

    layout->addWidget(new QLabel("Fog density:"), 2, 0, 1, 1);
    layout->addWidget(fogDensitySlider_, 2, 1, 1, 1);

    layout->addWidget(new QLabel("Fog contribution:"), 3, 0, 1, 1);
    layout->addWidget(fogContributionSlider_, 3, 1, 1, 1);

    layout->addWidget(labelBackgroundColorButton_, 4, 0, 1, 2);
    layout->addWidget(labelTextColorButton_, 5, 0, 1, 2);

    layout->setRowStretch(6, 1);

    setLayout(layout);
}

void DisplayParametersWidget::initializeSignals()
{
    connect(backgroundColorButton_, &QPushButton::clicked, [this](bool) {
        QColor defaultColor = vizWidget_->backgroundColor();
        QColor color = QColorDialog::getColor(defaultColor, this, "Pick color");
        if (color.isValid())
            vizWidget_->setBackgroundColor(color);
    });

    connect(fogDensitySlider_,
            static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
            [this](double value) {
        vizWidget_->setFogDensity((float)value);
    });

    connect(fogContributionSlider_,
            static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
            [this](double value) {
        vizWidget_->setFogContribution((float)value);
    });

    connect(labelBackgroundColorButton_, &QPushButton::clicked, [this](bool) {
        QColor defaultColor = vizWidget_->labelBackgroundColor();
        QColor color = QColorDialog::getColor(defaultColor, this, "Pick color",
                                              QColorDialog::ShowAlphaChannel);
        if (color.isValid())
            vizWidget_->setLabelBackgroundColor(color);
    });

    connect(labelTextColorButton_, &QPushButton::clicked, [this](bool) {
        QColor defaultColor = vizWidget_->labelTextColor();
        QColor color = QColorDialog::getColor(defaultColor, this, "Pick color",
                                              QColorDialog::ShowAlphaChannel);
        if (color.isValid())
            vizWidget_->setLabelTextColor(color);
    });
}
