#include "attributes.h"
#include "ui_attributes.h"
#include "treemodel.h"

Attributes::Attributes(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Attributes),
    model(nullptr)
{
    ui->setupUi(this);

    // connect vie
    connect(ui->comboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), [this](int index) {
        for (auto& r : rows)
            model->setVisibility(r, (Visibility)index, Editor);
    });

    // connect vir
    connect(ui->comboBox_2, QOverload<int>::of(&QComboBox::currentIndexChanged), [this](int index) {
        for (auto& r : rows)
            model->setVisibility(r, (Visibility)index, Renderer);
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
    if (selectedRows.isEmpty())
        return hide();

    model = selectedModel;
    rows = selectedRows;

    connect(model, &TreeModel::attributeChanged, this, &Attributes::updateSelection);

    // set title
    QString title = QString::number(rows.count()) + " elements ";

    QString list;

    for (const auto& r : rows)
        list += r.data().toString() + ", ";

    list.chop(2);

    ui->label->setTitle(title);
    ui->label->setElements(list);

    updateSelection();

    // set coordinates and camera origin
    QVector3D g;

    for (const auto& r : rows)
        g += reinterpret_cast<TreeItem*>(r.internalPointer())->getPosition();

    g /= rows.count();

    Camera::setOrigin(g);

    show();
}

void Attributes::unsetSelection()
{
    if (model) model->disconnect(this);
    model = nullptr;
}

void Attributes::updateSelection()
{
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
        ui->lineEdit->setText(name);

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
