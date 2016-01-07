#include "SelectionOperationsWidget.hpp"
#include "VizWidget.hpp"

SelectionOperationsWidget::SelectionOperationsWidget(VizWidget * vizWidget, QWidget * parent)
    : QWidget(parent)
    , vizWidget_(vizWidget)
{
    initializeControls();
    initializeSignals();
}

SelectionOperationsWidget::~SelectionOperationsWidget()
{

}

void SelectionOperationsWidget::initializeControls()
{
    auto * label1 = new QLabel("Select:");
    selectAllButton_ = new QPushButton("All");
    selectTypeButton_ = new QPushButton("Type");

    auto * label2 = new QLabel("Set:");
    setColorButton_ = new QPushButton("Color");
    setOpacityButton_ = new QPushButton("Opacity");
    setSizeButton_ = new QPushButton("Size");
    setLabelButton_ = new QPushButton("Label");

    auto * mainLayout = new QVBoxLayout();
    mainLayout->addWidget(label1);
    mainLayout->addWidget(selectAllButton_);
    mainLayout->addWidget(selectTypeButton_);
    mainLayout->addWidget(label2);
    mainLayout->addWidget(setColorButton_);
    mainLayout->addWidget(setOpacityButton_);
    mainLayout->addWidget(setSizeButton_);
    mainLayout->addWidget(setLabelButton_);
    setLayout(mainLayout);
}

void SelectionOperationsWidget::initializeSignals()
{
    connect(selectAllButton_, &QPushButton::clicked, [this](bool) {
        vizWidget_->setVisibleSelection(vizWidget_->allSelection());
    });

    connect(selectTypeButton_, &QPushButton::clicked, [this](bool) {
        bool ok = false;
        QString text = QInputDialog::getText(this, "Input type",
                                             "Enter the type of the atom to select:",
                                             QLineEdit::Normal,
                                             QString(),
                                             &ok);
        if (ok)
            vizWidget_->setVisibleSelection(vizWidget_->atomTypeSelection(text.toStdString()));
    });

    connect(setColorButton_, &QPushButton::clicked, [this](bool) {
        QColor color = QColorDialog::getColor(Qt::white, this, "Pick color");
        if (color.isValid())
            vizWidget_->selectedSpheresObject().setColor(color);
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
