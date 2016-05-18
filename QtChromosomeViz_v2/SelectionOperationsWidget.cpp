#include "SelectionOperationsWidget.hpp"
#include "VizWidget.hpp"

SelectionOperationsWidget::SelectionOperationsWidget(QWidget * parent)
    : QWidget(parent)
{
    initializeControls();
}

SelectionOperationsWidget::~SelectionOperationsWidget()
{

}

void SelectionOperationsWidget::setVizWidget(VizWidget *vizWidget)
{
    vizWidget_ = vizWidget;
    initializeSignals();
}

void SelectionOperationsWidget::initializeControls()
{
    auto * label2 = new QLabel("Set:");
    setColorButton_ = new QPushButton("Color");
    setSpecularColorButton_ = new QPushButton("Specular Color");
    setSpecularExponentButton_ = new QPushButton("Shininess");
    setOpacityButton_ = new QPushButton("Opacity");
    setSizeButton_ = new QPushButton("Size");
    setLabelButton_ = new QPushButton("Label");

    auto * mainLayout = new QVBoxLayout();
    mainLayout->addWidget(label2);
    mainLayout->addWidget(setColorButton_);
    mainLayout->addWidget(setSpecularColorButton_);
    mainLayout->addWidget(setSpecularExponentButton_);
    mainLayout->addWidget(setOpacityButton_);
    mainLayout->addWidget(setSizeButton_);
    mainLayout->addWidget(setLabelButton_);
    setLayout(mainLayout);
}

void SelectionOperationsWidget::initializeSignals()
{
    connect(setColorButton_, &QPushButton::clicked, [this](bool) {
        QColor color = QColorDialog::getColor(Qt::white, this, "Pick color");
        if (color.isValid())
            vizWidget_->selectedSpheresObject().setColor(color);
    });

    connect(setSpecularColorButton_, &QPushButton::clicked, [this](bool) {
        QColor color = QColorDialog::getColor(Qt::white, this, "Pick color");
        if (color.isValid())
            vizWidget_->selectedSpheresObject().setSpecularColor(color);
    });

    connect(setSpecularExponentButton_, &QPushButton::clicked, [this](bool) {
        bool ok = false;
        double shininess = QInputDialog::getDouble(this, "Input shininess",
                                                   "Enter shininess (between 1.0 and 100.0):",
                                                   10.0, 1.0, 100.0, 2, &ok);
        if (ok)
            vizWidget_->selectedSpheresObject().setSpecularExponent(shininess);
    });

    connect(setOpacityButton_, &QPushButton::clicked, [this](bool) {
        bool ok = false;
        double opacity = QInputDialog::getDouble(this, "Input opacity",
                                                 "Enter opacity (between 0.0 and 1.0):",
                                                 0.5, 0.0, 1.0, 2, &ok);
        if (ok)
            vizWidget_->selectedSpheresObject().setAlpha(opacity);
    });

    connect(setSizeButton_, &QPushButton::clicked, [this](bool) {
        bool ok = false;
        double size = QInputDialog::getDouble(this, "Input size",
                                                 "Enter sphere size:",
                                                 0.5, 0.0, 1e9, 2, &ok);
        if (ok)
            vizWidget_->selectedSpheresObject().setSize(size);
    });

    connect(setLabelButton_, &QPushButton::clicked, [this](bool) {
        bool ok = false;
        QString label = QInputDialog::getText(this, "Input label",
                                              "Enter the label you want to display on selected atoms:",
                                              QLineEdit::Normal,
                                              QString(),
                                              &ok);
        if (ok)
            vizWidget_->selectedSpheresObject().setLabel(label);
    });
}
