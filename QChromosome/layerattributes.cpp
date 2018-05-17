#include "layerattributes.h"
#include "ui_layerattributes.h"
#include "treemodel.h"

LayerAttributes::LayerAttributes(QWidget *parent) :
    MetaAttributes(parent),
    ui(new Ui::LayerAttributes),
    model(nullptr)
{
    ui->setupUi(this);

    // connect name
    connect(ui->lineEdit, &QLineEdit::editingFinished, [this] {
        model->setName(rows, ui->lineEdit->text());
    });

    // connect vie
    connect(ui->comboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), [this](int index) {
        model->setVisibility(rows, (Visibility)index, Editor);
    });

    // connect vir
    connect(ui->comboBox_2, QOverload<int>::of(&QComboBox::currentIndexChanged), [this](int index) {
        model->setVisibility(rows, (Visibility)index, Renderer);
    });
}

LayerAttributes::~LayerAttributes()
{
    delete ui;
}

void LayerAttributes::setSelection(TreeModel* selectedModel, const QModelIndexList &selectedRows)
{
    Q_ASSERT(!selectedRows.empty());

    ui->tabWidget->setCurrentIndex(0);

    model = selectedModel;
    rows = selectedRows;

    layers.clear();
    layers.reserve(selectedRows.size());

    for (const auto& i : selectedRows)
        layers.append(reinterpret_cast<LayerItem*>(i.internalPointer()));

    connect(model, &TreeModel::attributeChanged, this, &LayerAttributes::updateModelSelection);

    // set title
    QString title("Layer object ");

    if (rows.count() > 1)
        title += "(" + QString::number(rows.count()) + " elements) ";

    ui->label->setTitle(title);

    updateModelSelection();

    updatePosition();
}

void LayerAttributes::unsetSelection()
{
    if (model) model->disconnect(this);
    model = nullptr;
}

void LayerAttributes::updateModelSelection()
{
    // set elements
    QString list;

    for (const auto& r : rows)
        list += r.data().toString() + ", ";

    list.chop(2);

    ui->label->setElements(list);

    auto fst = rows.first();
    bool multiple;

    // set name
    QString name = fst.data().toString();
    multiple = false;

    for (const auto& r : rows)
        if (name != r.data().toString())
        {
            multiple = true;
            break;
        }

    if (multiple)
        ui->lineEdit->setMultipleValues();
    else
        ui->lineEdit->setText(name, false);

    // set vie
    int vie = fst.sibling(fst.row(), 3).data().toInt();
    multiple = false;

    for (const auto& r : rows)
        if (vie != r.sibling(r.row(), 3).data().toInt())
        {
            multiple = true;
            break;
        }

    if (multiple)
        ui->comboBox->setMultipleValues();
    else
        ui->comboBox->setCurrentIndex(vie, false);

    // set vir
    int vir = fst.sibling(fst.row(), 4).data().toInt();
    multiple = false;

    for (const auto& r : rows)
        if (vir != r.sibling(r.row(), 4).data().toInt())
        {
            multiple = true;
            break;
        }

    if (multiple)
        ui->comboBox_2->setMultipleValues();
    else
        ui->comboBox_2->setCurrentIndex(vir, false);
}

#include "treeitem.h"
#include "camera.h"

void LayerAttributes::updatePosition()
{
    auto fst = layers.first();
    bool multiple;

    // set P.X
    double x = fst->getPosition().x();
    multiple = false;

    for (const auto& l : layers)
        if (x != l->getPosition().x())
        {
            multiple = true;
            break;
        }

    if (multiple)
        ui->doubleSpinBox->setMultipleValues();
    else
        ui->doubleSpinBox->setValue(x, false);

    // set P.Y
    double y = fst->getPosition().y();
    multiple = false;

    for (const auto& l : layers)
        if (y != l->getPosition().y())
        {
            multiple = true;
            break;
        }

    if (multiple)
        ui->doubleSpinBox_2->setMultipleValues();
    else
        ui->doubleSpinBox_2->setValue(y, false);

    // set P.Z
    double z = fst->getPosition().z();
    multiple = false;

    for (const auto& l : layers)
        if (z != l->getPosition().z())
        {
            multiple = true;
            break;
        }

    if (multiple)
        ui->doubleSpinBox_3->setMultipleValues();
    else
        ui->doubleSpinBox_3->setValue(z, false);
}
