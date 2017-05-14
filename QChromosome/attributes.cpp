#include "attributes.h"
#include "ui_attributes.h"

Attributes::Attributes(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Attributes)
{
    ui->setupUi(this);

    // set name
    connect(ui->lineEdit, &QLineEdit::editingFinished, [this] {
        if (ui->lineEdit->isModified())
        {
            if (ui->lineEdit->text().isEmpty())
                updateName();
            else
                vizWidget_->selectedSpheresObject().setName(ui->lineEdit->text());
        }
    });

    // set label
    connect(ui->lineEdit_2, &QLineEdit::editingFinished, [this] {
        if (ui->lineEdit_2->isModified())
            vizWidget_->selectedSpheresObject().setLabel(ui->lineEdit_2->text());
    });

    // set vie
    connect(ui->comboBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [this](int i) {
        vizWidget_->selectedSpheresObject().setVisible(Visibility(i), Editor);
    });

    // set vir
    connect(ui->comboBox_2, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [this](int i) {
        vizWidget_->selectedSpheresObject().setVisible(Visibility(i), Renderer);
    });

    // set radius
    connect(ui->doubleSpinBox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [this] (double val) {
        vizWidget_->selectedSpheresObject().setSize(val);
    });

    // set segments

}

Attributes::~Attributes()
{
    delete ui;
}

void Attributes::setVizWidget(VizWidget *vizWidget)
{
    vizWidget_ = vizWidget;
}

void Attributes::handleSelection(const AtomSelection &selection)
{
    if (!selection.atomCount())
        return hide();

    // set title
    title = "Atom object ";

    if (selection.atomCount() > 1)
        title += "(" + QString::number(selection.atomCount()) + " elements) ";

    list.clear();

    for (auto i : selection.selectedIndices())
        list += "Atom." + QString::number(i + 1) + ", ";

    list.chop(2);

    ui->label_14->setText(title + "[" + ui->label_14->fontMetrics().elidedText(list, Qt::ElideRight, width() - ui->label_14->fontMetrics().width(title + "[]") - 58) + "]");

    // set name
    auto l = selection.getName();
    ui->lineEdit->setText(l.isValid() ? l.toString() : "<< multiple values >>");

    // set label
    l = selection.getLabel();
    ui->lineEdit_2->setText(l.isValid() ? l.toString() : "<< multiple values >>");

    // set vie
    ui->comboBox->setCurrentIndex(selection.getVisibility(Editor), false);

    // set vir
    ui->comboBox->setCurrentIndex(selection.getVisibility(Renderer), false);

    // set coordinates
    auto c = selection.getCoordinates();

    ui->spinBox->setValue(std::get<0>(c), false);
    ui->spinBox_2->setValue(std::get<1>(c), false);
    ui->spinBox_3->setValue(std::get<2>(c), false);

    // set radius
    ui->doubleSpinBox->setValue(selection.getSize(), false);

    // set segments

    show();
}

void Attributes::resizeEvent(QResizeEvent *event)
{
    ui->label_14->setText(title + "[" + ui->label_14->fontMetrics().elidedText(list, Qt::ElideRight, width() - ui->label_14->fontMetrics().width(title + "[]") - 58) + "]");

    QWidget::resizeEvent(event);
}

void Attributes::updateName()
{
    auto l = vizWidget_->selectedSpheresObject().getName();
    ui->lineEdit->setText(l.isValid() ? l.toString() : "<< multiple values >>");
}

void Attributes::updateVisibility(VisibilityMode m)
{
    if (m == Editor)
        ui->comboBox->setCurrentIndex(vizWidget_->selectedSpheresObject().getVisibility(Editor), false);
    else
        ui->comboBox_2->setCurrentIndex(vizWidget_->selectedSpheresObject().getVisibility(Renderer), false);
}
