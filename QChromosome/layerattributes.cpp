#include "layerattributes.h"
#include "ui_layerattributes.h"

LayerAttributes::LayerAttributes(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LayerAttributes)
{
    ui->setupUi(this);

    connect(ui->pushButton_2, SIGNAL(clicked(bool)), this, SLOT(appendLayer()));


    // ATTRIBUTES
    // set radius
    connect(ui->doubleSpinBox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [this] () {
        ui->doubleSpinBox->setSpecialValueText("");
    });

    connect(ui->doubleSpinBox, &QDoubleSpinBox::editingFinished, [this] {
        if (ui->doubleSpinBox->specialValueText().isEmpty())
            vizWidget_->selectedSpheresObject().setSize(ui->doubleSpinBox->value());
    });

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

LayerAttributes::~LayerAttributes()
{
    delete ui;
}

void LayerAttributes::setSimulation(std::shared_ptr<Simulation> s, VizWidget *vw)
{
    simulation = s;
    vizWidget_ = vw;
}

void LayerAttributes::handleSelection(const QList<unsigned int> &s, const AtomSelection& selection)
{
    if (!s.count())
        return hide();

    // set title
    title = "Layer object ";

    if (s.count() > 1)
        title += "(" + QString::number(s.count()) + " elements) ";

    list.clear();

    for (auto i : s)
        list += QString("Layer") + (i ? QString(".") + QString::number(i + 1) : "") + ", ";

    list.chop(2);

    ui->label_14->setText(title + "[" + ui->label_14->fontMetrics().elidedText(list, Qt::ElideRight, width() - ui->label_14->fontMetrics().width(title + "[]") - 58) + "]");

    ui->pushButton_2->setVisible(s.count() == 1);

    id = s.first();


    // ATTRIBUTES
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

void LayerAttributes::resizeEvent(QResizeEvent *event)
{
    ui->label_14->setText(title + "[" + ui->label_14->fontMetrics().elidedText(list, Qt::ElideRight, width() - ui->label_14->fontMetrics().width(title + "[]") - 58) + "]");

    QWidget::resizeEvent(event);
}

#include <QFileDialog>
#include "../QtChromosomeViz_v2/bartekm_code/PDBSimulationLayer.h"
#include "../QtChromosomeViz_v2/bartekm_code/ProtobufSimulationlayer.h"
#include <QMessageBox>

void LayerAttributes::appendLayer()
{
    try {
        QString path = QFileDialog::getOpenFileName(this, "", "/home", "Simulation file (*.pdb *.bin)");

        if (!path.isEmpty())
        {
            std::shared_ptr<SimulationLayer> simulationLayer;

            if (path.endsWith(".pdb"))
                simulationLayer = std::make_shared<PDBSimulationLayer>(path.toStdString());
            else
                simulationLayer = std::make_shared<ProtobufSimulationLayer>(path.toStdString());

            simulation->getSimulationLayerConcatenation(id)->appendSimulationLayer(simulationLayer);
        }
    } catch (std::exception& e) {
        QMessageBox::critical(this, "Error occured.", e.what());
    }
}
