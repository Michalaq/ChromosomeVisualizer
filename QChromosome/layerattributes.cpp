#include "layerattributes.h"
#include "ui_layerattributes.h"

LayerAttributes::LayerAttributes(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LayerAttributes)
{
    ui->setupUi(this);

    // set name

    // set label
    connect(ui->lineEdit_2, &QLineEdit::editingFinished, [this] {
        if (ui->lineEdit_2->isModified())
            vizWidget_->selectedSpheresObject().setLabel(ui->lineEdit_2->text());
    });

    // set vie

    // set vir

    // set radius
    connect(ui->doubleSpinBox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [this] (double val) {
        vizWidget_->selectedSpheresObject().setSize(val);
    });

    // set segments

    connect(ui->pushButton_2, SIGNAL(clicked(bool)), this, SLOT(appendLayer()));
}

LayerAttributes::~LayerAttributes()
{
    delete ui;
}

void LayerAttributes::setVizWidget(VizWidget *vizWidget)
{
    vizWidget_ = vizWidget;
}

void LayerAttributes::setSimulation(std::shared_ptr<Simulation> s)
{
    simulation = s;
}

void LayerAttributes::handleSelection(const AtomSelection &selection, const QList<unsigned int> &layers)
{
    if (!layers.count())
        return hide();

    // set title
    title = "Layer object ";

    if (layers.count() > 1)
        title += "(" + QString::number(layers.count()) + " elements) ";

    list.clear();

    for (auto i : layers)
        list += QString("Layer") + (i ? QString(".") + QString::number(i + 1) : "") + ", ";

    list.chop(2);

    ui->label_14->setText(title + "[" + ui->label_14->fontMetrics().elidedText(list, Qt::ElideRight, width() - ui->label_14->fontMetrics().width(title + "[]") - 58) + "]");

    // set name

    // set label
    auto l = selection.getLabel();

    ui->lineEdit_2->setText(l.isValid() ? l.toString() : "<< multiple values >>");

    // set vie
    ui->comboBox->setCurrentIndex(selection.getVisibility(Editor), false);

    // set vir
    ui->comboBox_2->setCurrentIndex(selection.getVisibility(Renderer), false);

    // set coordinates
    auto c = selection.getCoordinates();

    ui->spinBox->setValue(std::get<0>(c), false);
    ui->spinBox_2->setValue(std::get<1>(c), false);
    ui->spinBox_3->setValue(std::get<2>(c), false);

    // set radius
    ui->doubleSpinBox->setValue(selection.getSize(), false);

    // set segments

    show();

    ui->pushButton_2->setVisible(layers.count() == 1);

    id = layers.first();
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
#include <QStandardPaths>

void LayerAttributes::appendLayer()
{
    try {
        QString path = QFileDialog::getOpenFileName(0, "", QStandardPaths::writableLocation(QStandardPaths::HomeLocation), "Simulation file (*.pdb *.bin)");

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
        QMessageBox::critical(0, "Error occured.", e.what());
    }
}
