#include "seriesattributes.h"
#include "ui_seriesattributes.h"
#include "treemodel.h"

SeriesAttributes::SeriesAttributes(QWidget *parent) :
    MetaAttributes(parent),
    ui(new Ui::SeriesAttributes),
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

    // connect color
    connect(ui->comboBox_3, &Picker::valueChanged, [this](QColor val) {
        for (auto s : series)
            s->setColor(val);
    });

    // connect opacity
    connect(ui->doubleSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [this](double val) {
        for (auto s : series)
            s->setOpacity(val / 100);
    });
}

SeriesAttributes::~SeriesAttributes()
{
    delete ui;
}

void SeriesAttributes::setSelection(TreeModel* selectedModel, const QModelIndexList &selectedRows)
{
    Q_ASSERT(!selectedRows.empty());

    ui->tabWidget->setCurrentIndex(0);

    model = selectedModel;
    rows = selectedRows;

    series.clear();
    series.reserve(selectedRows.size());

    for (const auto& i : selectedRows)
        series.append(reinterpret_cast<SeriesItem*>(i.internalPointer()));

    connect(model, &TreeModel::attributeChanged, this, &SeriesAttributes::updateModelSelection);

    // set title
    QString title("Series object ");

    if (rows.count() > 1)
        title += "(" + QString::number(rows.count()) + " elements) ";

    ui->label->setTitle(title);

    updateModelSelection();

    auto fst = series.first();
    bool multiple;

    // set color
    QColor c = fst->getColor();
    multiple = false;

    for (const auto& s : series)
        if (c != s->getColor())
        {
            multiple = true;
            break;
        }

    if (multiple)
        ui->comboBox_3->setMultipleValues();
    else
        ui->comboBox_3->setValue(c, false);

    // set opacity
    qreal o = fst->getOpacity();
    multiple = false;

    for (const auto& s : series)
        if (o != s->getOpacity())
        {
            multiple = true;
            break;
        }

    if (multiple)
        ui->doubleSpinBox->setMultipleValues();
    else
        ui->doubleSpinBox->setValue(o * 100, false);
}

void SeriesAttributes::unsetSelection()
{
    if (model) model->disconnect(this);
    model = nullptr;
}

void SeriesAttributes::updateModelSelection()
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
