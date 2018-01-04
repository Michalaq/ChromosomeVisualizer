#include "attributes.h"
#include "ui_attributes.h"
#include "treemodel.h"

Attributes::Attributes(QWidget *parent) :
    QWidget(parent),
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
}

Attributes::~Attributes()
{
    delete ui;
}

#include "treeitem.h"
#include "camera.h"

void Attributes::setSelection(TreeModel* selectedModel, const QModelIndexList &selectedRows)
{
    Q_ASSERT(!selectedRows.empty());

    model = selectedModel;
    rows = selectedRows;

    connect(model, &TreeModel::attributeChanged, this, &Attributes::updateModelSelection);

    // set title
    QString title = QString::number(rows.count()) + " elements ";

    ui->label->setTitle(title);

    updateModelSelection();

    updatePosition();
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

    // set name
    QString name = fst.data().toString();

    for (const auto& r : rows)
        if (name != r.data().toString())
        {
            name.clear();
            break;
        }

    if (name.isEmpty())
        ui->lineEdit->setMultipleValues();
    else
        ui->lineEdit->setText(name, false);

    // set vie
    int vie = fst.sibling(fst.row(), 3).data().toInt();

    for (const auto& r : rows)
        if (vie != r.sibling(r.row(), 3).data().toInt())
        {
            vie = -1;
            break;
        }

    if (vie == -1)
        ui->comboBox->setMultipleValues();
    else
        ui->comboBox->setCurrentIndex(vie, false);

    // set vir
    int vir = fst.sibling(fst.row(), 4).data().toInt();

    for (const auto& r : rows)
        if (vir != r.sibling(r.row(), 4).data().toInt())
        {
            vir = -1;
            break;
        }

    if (vir == -1)
        ui->comboBox_2->setMultipleValues();
    else
        ui->comboBox_2->setCurrentIndex(vir, false);
}

void Attributes::updatePosition()
{
    auto fst = rows.first();

    // set P.X
    double x = reinterpret_cast<TreeItem*>(fst.internalPointer())->getPosition().x();

    for (const auto& r : rows)
        if (x != reinterpret_cast<TreeItem*>(r.internalPointer())->getPosition().x())
        {
            x = qSNaN();
            break;
        }

    if (qIsNaN(x))
        ui->doubleSpinBox->setMultipleValues();
    else
        ui->doubleSpinBox->setValue(x, false);

    // set P.Y
    double y = reinterpret_cast<TreeItem*>(fst.internalPointer())->getPosition().y();

    for (const auto& r : rows)
        if (y != reinterpret_cast<TreeItem*>(r.internalPointer())->getPosition().y())
        {
            y = qSNaN();
            break;
        }

    if (qIsNaN(y))
        ui->doubleSpinBox_2->setMultipleValues();
    else
        ui->doubleSpinBox_2->setValue(y, false);

    // set P.Z
    double z = reinterpret_cast<TreeItem*>(fst.internalPointer())->getPosition().z();

    for (const auto& r : rows)
        if (z != reinterpret_cast<TreeItem*>(r.internalPointer())->getPosition().z())
        {
            z = qSNaN();
            break;
        }

    if (qIsNaN(z))
        ui->doubleSpinBox_3->setMultipleValues();
    else
        ui->doubleSpinBox_3->setValue(z, false);

    // set camera origin
    QVector3D g;

    for (const auto& r : rows)
        g += reinterpret_cast<TreeItem*>(r.internalPointer())->getPosition();

    g /= rows.count();

    Camera::setOrigin(g);
}
