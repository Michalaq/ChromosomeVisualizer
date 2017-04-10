#include "attributes.h"
#include "ui_attributes.h"

Attributes::Attributes(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Attributes)
{
    ui->setupUi(this);

    // set name

    // set label
    connect(ui->lineEdit_2, &QLineEdit::editingFinished, [this] {
        if (ui->lineEdit_2->isModified())
            vizWidget_->selectedSpheresObject().setLabel(ui->lineEdit_2->text());
    });

    // set vie
    connect(ui->comboBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [this](int i) {
        switch (i) {
        case 0: // default
            break;
        case 1: // on
            vizWidget_->selectedSpheresObject().setVisible(true);
            break;
        case 2: // off
            vizWidget_->selectedSpheresObject().setVisible(false);
            break;
        }
    });

    // set vir

    // set radius
    connect(ui->doubleSpinBox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [this] (double val) {
        vizWidget_->selectedSpheresObject().setSize(val);
    });

    // set segments

    // set color
    connect(ui->widget, &Picker::valueChanged, [this] (QColor color) {
        vizWidget_->selectedSpheresObject().setColor(color);
    });

    // set transparency
    connect(ui->doubleSpinBox_2, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [this] (double val) {
        vizWidget_->selectedSpheresObject().setAlpha((100. - val) / 100);
    });

    // set specular width
    connect(ui->doubleSpinBox_3, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [this] (double val) {
        vizWidget_->selectedSpheresObject().setSpecularExponent(val);
    });

    // set specular color
    connect(ui->widget_2, &Picker::valueChanged, [this] (QColor color) {
        vizWidget_->selectedSpheresObject().setSpecularColor(color);
    });
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

    // set label
    auto l = selection.getLabel();

    ui->lineEdit_2->setText(l.isValid() ? l.toString() : "<< multiple values >>");

    // set vie
    ui->comboBox->setCurrentIndex(selection.getVisibility(), false);

    // set coordinates
    auto c = selection.getCoordinates();

    ui->spinBox->setValue(std::get<0>(c), false);
    ui->spinBox_2->setValue(std::get<1>(c), false);
    ui->spinBox_3->setValue(std::get<2>(c), false);

    // set radius
    ui->doubleSpinBox->setValue(selection.getSize(), false);

    // set segments

    // set color
    ui->widget->setValue(selection.getColor(), false);

    // set transparency
    ui->doubleSpinBox_2->setValue(selection.getAlpha(), false);

    // set specular width
    ui->doubleSpinBox_3->setValue(selection.getSpecularExponent(), false);

    // set specular color
    ui->widget_2->setValue(selection.getSpecularColor(), false);

    show();
}

void Attributes::resizeEvent(QResizeEvent *event)
{
    ui->label_14->setText(title + "[" + ui->label_14->fontMetrics().elidedText(list, Qt::ElideRight, width() - ui->label_14->fontMetrics().width(title + "[]") - 58) + "]");

    QWidget::resizeEvent(event);
}
