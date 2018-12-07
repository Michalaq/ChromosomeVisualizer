#include "atomattributes.h"
#include "ui_atomattributes.h"
#include "treemodel.h"

QFontStyledDelegate::QFontStyledDelegate(QObject *parent) : QStyledItemDelegate(parent)
{

}

void QFontStyledDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyleOptionViewItem opt = option;
    initStyleOption(&opt, index);

    QString text = index.data(Qt::DisplayRole).toString();

    opt.font.setBold(text.contains("Bold"));
    opt.font.setItalic(text.contains("Italic"));

    QStyledItemDelegate::paint(painter, opt, index);
}

#include "session.h"

AtomAttributes::AtomAttributes(QWidget *parent) :
    MetaAttributes(parent),
    ui(new Ui::AtomAttributes),
    model(nullptr)
{
    ui->setupUi(this);

    ui->comboBox_4->setItemDelegate(new QFontStyledDelegate(this));

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
        for (auto a : atoms)
            a->setCylinderRadius(val);
        ui->doubleSpinBox_4->setMinimum(val);
        emit attributeChanged();
    });

    // connect sphere radius
    connect(ui->doubleSpinBox_4, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [this](double val) {
        for (auto a : atoms)
            a->setSphereRadius(val);
        ui->doubleSpinBox_5->setMaximum(val);
        emit attributeChanged();
    });

    // connect label
    connect(ui->textEdit, &QTextEdit::textChanged, this, &AtomAttributes::updateLabels);
    connect(ui->comboBox_4, &Picker::valueChanged, this, &AtomAttributes::updateLabels);
    connect(ui->comboBox_6, &Picker::valueChanged, this, &AtomAttributes::updateLabels);
    connect(ui->fontComboBox, &FontComboBox::valueChanged, this, &AtomAttributes::updateLabels);
    connect(ui->comboBox_3, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &AtomAttributes::updateLabels);
    connect(ui->spinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &AtomAttributes::updateLabels);

    ui->comboBox_4->setValue(QColor(0, 0, 0, 76), false);
    ui->comboBox_6->setValue(Qt::white, false);
    ui->spinBox->setValue(9, false);
}

AtomAttributes::~AtomAttributes()
{
    delete ui;
}

void AtomAttributes::setSelection(TreeModel* selectedModel, const QModelIndexList &selectedRows)
{
    Q_ASSERT(!selectedRows.empty());

    ui->tabWidget->setCurrentIndex(0);

    model = selectedModel;
    rows = selectedRows;

    atoms.clear();
    atoms.reserve(selectedRows.size());

    for (const auto& i : selectedRows)
        atoms.append(reinterpret_cast<AtomItem*>(i.internalPointer()));

    connect(model, &TreeModel::attributeChanged, this, &AtomAttributes::updateModelSelection);

    // set title
    QString title("Atom object ");

    if (rows.count() > 1)
        title += "(" + QString::number(rows.count()) + " elements) ";

    ui->label->setTitle(title);

    updateModelSelection();

    updatePosition();

    auto fst = atoms.first();
    bool multiple;

    // set cylinder radius
    double cr = fst->getCylinderRadius();
    multiple = false;

    for (const auto& a : atoms)
        if (cr != a->getCylinderRadius())
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

    for (const auto& a : atoms)
        if (sr != a->getSphereRadius())
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

    // set label
    QString l = fst->getLabel();
    multiple = false;

    for (const auto& a : atoms)
        if (l != a->getLabel())
        {
            multiple = true;
            break;
        }

    if (multiple)
        ui->textEdit->setMultipleValues();
    else
        ui->textEdit->setText(l, false);
}

void AtomAttributes::unsetSelection()
{
    if (model) model->disconnect(this);
    model = nullptr;
}

void AtomAttributes::updateModelSelection()
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

void AtomAttributes::updatePosition()
{
    if (atoms.empty())
        return;

    auto fst = atoms.first();
    bool multiple;

    // set P.X
    double x = fst->getPosition().x();
    multiple = false;

    for (const auto& a : atoms)
        if (x != a->getPosition().x())
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

    for (const auto& a : atoms)
        if (y != a->getPosition().y())
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

    for (const auto& a : atoms)
        if (z != a->getPosition().z())
        {
            multiple = true;
            break;
        }

    if (multiple)
        ui->doubleSpinBox_3->setMultipleValues();
    else
        ui->doubleSpinBox_3->setValue(z, false);
}

void AtomAttributes::updateLabels()
{
    const QString& val = ui->textEdit->toPlainText();
    auto r = session->labelAtlas.addLabel(ui);

    for (auto a : atoms)
        a->setLabel(val, r);

    emit attributeChanged();
}
