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
    connect(ui->doubleSpinBox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [this] () {
        ui->doubleSpinBox->setSpecialValueText("");
    });

    connect(ui->doubleSpinBox, &QDoubleSpinBox::editingFinished, [this] {
        if (ui->doubleSpinBox->specialValueText().isEmpty())
            vizWidget_->selectedSpheresObject().setSize(ui->doubleSpinBox->value());
    });

    // set segments

    // set color
    connect(ui->widget, &Picker::valueChanged, [this] (QColor color) {
        vizWidget_->selectedSpheresObject().setColor(color);
    });

    // set transparency
    connect(ui->doubleSpinBox_2, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [this] () {
        ui->doubleSpinBox_2->setSpecialValueText("");
    });

    connect(ui->doubleSpinBox_2, &QDoubleSpinBox::editingFinished, [this] {
        if (ui->doubleSpinBox_2->specialValueText().isEmpty())
            vizWidget_->selectedSpheresObject().setAlpha((100. - ui->doubleSpinBox_2->value()) / 100);
    });

    // set specular width
    connect(ui->doubleSpinBox_3, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [this] () {
        ui->doubleSpinBox_3->setSpecialValueText("");
    });

    connect(ui->doubleSpinBox_3, &QDoubleSpinBox::editingFinished, [this] {
        if (ui->doubleSpinBox_3->specialValueText().isEmpty())
            vizWidget_->selectedSpheresObject().setSpecularExponent(ui->doubleSpinBox_3->value());
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
    ui->comboBox->setCurrentIndex(selection.getVisibility());

    // set coordinates
    auto c = selection.getCoordinates();

    auto x = c.at(0);

    if (x.isValid())
    {
        ui->spinBox->setValue(x.toFloat());
        ui->spinBox->setSpecialValueText("");
    }
    else
    {
        ui->spinBox->setValue(ui->spinBox->minimum());
        ui->spinBox->setSpecialValueText("<< multiple values >>");
    }

    auto y = c.at(1);

    if (x.isValid())
    {
        ui->spinBox_2->setValue(y.toFloat());
        ui->spinBox_2->setSpecialValueText("");
    }
    else
    {
        ui->spinBox_2->setValue(ui->spinBox_2->minimum());
        ui->spinBox_2->setSpecialValueText("<< multiple values >>");
    }

    auto z = c.at(2);

    if (x.isValid())
    {
        ui->spinBox_3->setValue(z.toFloat());
        ui->spinBox_3->setSpecialValueText("");
    }
    else
    {
        ui->spinBox_3->setValue(ui->spinBox_3->minimum());
        ui->spinBox_3->setSpecialValueText("<< multiple values >>");
    }

    // set radius
    auto r = selection.getSize();

    if (r.isValid())
    {
        ui->doubleSpinBox->setValue(r.toFloat());
        ui->doubleSpinBox->setSpecialValueText("");
    }
    else
    {
        ui->doubleSpinBox->setValue(ui->doubleSpinBox->minimum());
        ui->doubleSpinBox->setSpecialValueText("<< multiple values >>");
    }

    // set segments

    // set color
    ui->widget->setValue(selection.getColor());

    // set transparency
    auto a = selection.getAlpha();

    if (a.isValid())
    {
        ui->doubleSpinBox_2->setValue(a.toFloat());
        ui->doubleSpinBox_2->setSpecialValueText("");
    }
    else
    {
        ui->doubleSpinBox_2->setValue(ui->doubleSpinBox_2->minimum());
        ui->doubleSpinBox_2->setSpecialValueText("<< multiple values >>");
    }

    // set specular width
    auto w = selection.getSpecularExponent();

    if (w.isValid())
    {
        ui->doubleSpinBox_3->setValue(w.toFloat());
        ui->doubleSpinBox_3->setSpecialValueText("");
    }
    else
    {
        ui->doubleSpinBox_3->setValue(ui->doubleSpinBox_3->minimum());
        ui->doubleSpinBox_3->setSpecialValueText("<< multiple values >>");
    }

    // set specular color
    ui->widget_2->setValue(selection.getSpecularColor());

    show();
}

void Attributes::resizeEvent(QResizeEvent *event)
{
    ui->label_14->setText(title + "[" + ui->label_14->fontMetrics().elidedText(list, Qt::ElideRight, width() - ui->label_14->fontMetrics().width(title + "[]") - 58) + "]");

    QWidget::resizeEvent(event);
}
