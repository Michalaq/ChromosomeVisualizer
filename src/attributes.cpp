#include "attributes.h"
#include "ui_attributes.h"
#include "treemodel.h"

Session* MetaAttributes::session;

MetaAttributes::MetaAttributes(QWidget *parent) : QWidget(parent)
{

}

MetaAttributes::~MetaAttributes()
{

}

void MetaAttributes::setSession(Session *s)
{
    session = s;
}

Attributes::Attributes(QWidget *parent) :
    MetaAttributes(parent),
    ui(new Ui::Attributes),
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

    // connect cylinder radius
    connect(ui->doubleSpinBox_5, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [this](double val) {
        for (auto i : items)
            i->setCylinderRadius(val);
        ui->doubleSpinBox_4->setMinimum(val);
        emit attributeChanged();
    });

    // connect sphere radius
    connect(ui->doubleSpinBox_4, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [this](double val) {
        for (auto i : items)
            i->setSphereRadius(val);
        ui->doubleSpinBox_5->setMaximum(val);
        emit attributeChanged();
    });
}

Attributes::~Attributes()
{
    delete ui;
}

void Attributes::setSelection(TreeModel* selectedModel, const QModelIndexList &selectedRows)
{
    Q_ASSERT(!selectedRows.empty());

    ui->tabWidget->setCurrentIndex(0);

    model = selectedModel;
    rows = selectedRows;

    items.clear();
    items.reserve(selectedRows.size());

    for (const auto& i : selectedRows)
        items.append(reinterpret_cast<TreeItem*>(i.internalPointer()));

    connect(model, &TreeModel::attributeChanged, this, &Attributes::updateModelSelection);

    // set title
    QString title = QString::number(rows.count()) + " elements ";

    ui->label->setTitle(title);

    updateModelSelection();

    updatePosition();

    auto fst = items.first();
    bool multiple;

    // set cylinder radius
    double cr = fst->getCylinderRadius();
    multiple = false;

    for (const auto& i : items)
        if (cr != i->getCylinderRadius())
        {
            multiple = true;
            break;
        }

    if (multiple)
        ui->doubleSpinBox_5->setMultipleValues();
    else
    {
        ui->doubleSpinBox_5->setValue(cr, false);
        ui->doubleSpinBox_4->setMinimum(cr, false);
    }

    // set sphere radius
    double sr = fst->getSphereRadius();
    multiple = false;

    for (const auto& i : items)
        if (sr != i->getSphereRadius())
        {
            multiple = true;
            break;
        }

    if (multiple)
        ui->doubleSpinBox_4->setMultipleValues();
    else
    {
        ui->doubleSpinBox_4->setValue(sr, false);
        ui->doubleSpinBox_5->setMaximum(sr, false);
    }
}

void Attributes::unsetSelection()
{
    if (model) model->disconnect(this);
    model = nullptr;
}

void Attributes::updateModelSelection()
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

void Attributes::updatePosition()
{
    auto fst = items.first();
    bool multiple;

    // set P.X
    double x = fst->getPosition().x();
    multiple = false;

    for (const auto& i : items)
        if (x != i->getPosition().x())
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

    for (const auto& i : items)
        if (y != i->getPosition().y())
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

    for (const auto& i : items)
        if (z != i->getPosition().z())
        {
            multiple = true;
            break;
        }

    if (multiple)
        ui->doubleSpinBox_3->setMultipleValues();
    else
        ui->doubleSpinBox_3->setValue(z, false);
}
