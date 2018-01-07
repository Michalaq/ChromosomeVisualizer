#include "cameraattributes.h"
#include "ui_cameraattributes.h"
#include "treemodel.h"
#include "camera.h"

CameraAttributes::CameraAttributes(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CameraAttributes),
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

    //connect P.X
    connect(ui->doubleSpinBox, QOverload<double>::of(&DoubleSpinBox::valueChanged), [this](double val) {
        for (auto c : cameras)
        {
            auto pos = c->position();
            pos.setX(val);
            c->setPosition(pos);
        }
    });

    //connect P.Y
    connect(ui->doubleSpinBox_2, QOverload<double>::of(&DoubleSpinBox::valueChanged), [this](double val) {
        for (auto c : cameras)
        {
            auto pos = c->position();
            pos.setY(val);
            c->setPosition(pos);
        }
    });

    //connect P.Z
    connect(ui->doubleSpinBox_3, QOverload<double>::of(&DoubleSpinBox::valueChanged), [this](double val) {
        for (auto c : cameras)
        {
            auto pos = c->position();
            pos.setZ(val);
            c->setPosition(pos);
        }
    });

    //connect R.H
    connect(ui->doubleSpinBox_4, QOverload<double>::of(&DoubleSpinBox::valueChanged), [this](double val) {
        for (auto c : cameras)
        {
            auto ang = c->EulerAngles();
            c->setEulerAgnles(val, ang.x(), ang.z());
        }
    });

    //connect R.P
    connect(ui->doubleSpinBox_5, QOverload<double>::of(&DoubleSpinBox::valueChanged), [this](double val) {
        for (auto c : cameras)
        {
            auto ang = c->EulerAngles();
            c->setEulerAgnles(ang.y(), val, ang.z());
        }
    });

    //connect R.B
    connect(ui->doubleSpinBox_6, QOverload<double>::of(&DoubleSpinBox::valueChanged), [this](double val) {
        for (auto c : cameras)
        {
            auto ang = c->EulerAngles();
            c->setEulerAgnles(ang.y(), ang.x(), val);
        }
    });
}

CameraAttributes::~CameraAttributes()
{
    delete ui;
}

#include "treeitem.h"

void CameraAttributes::setSelection(TreeModel* selectedModel, const QModelIndexList &selectedRows)
{
    Q_ASSERT(!selectedRows.empty());

    model = selectedModel;
    rows = selectedRows;

    cameras.clear();
    cameras.reserve(selectedRows.size());

    for (const auto& i : selectedRows)
        cameras.append(reinterpret_cast<CameraItem*>(i.internalPointer())->getCamera());

    connect(model, &TreeModel::attributeChanged, this, &CameraAttributes::updateModelSelection);

    for (const auto c : cameras)
    {
        connect(c, &Camera::modelViewChanged, this, &CameraAttributes::updateModelView);
        connect(c, &Camera::projectionChanged, this, &CameraAttributes::updateProjection);
    }

    // set title
    QString title("Camera object ");

    if (rows.count() > 1)
        title += "(" + QString::number(rows.count()) + " elements) ";

    ui->label->setTitle(title);

    updateModelSelection();

    updateModelView();

    updateProjection();
}

void CameraAttributes::unsetSelection()
{
    if (model) model->disconnect(this);
    model = nullptr;

    for (const auto c : cameras)
        c->disconnect(this);
}

void CameraAttributes::updateModelSelection()
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

void CameraAttributes::updateModelView()
{
    const auto fst = cameras.first();

    // set P.X
    double x = fst->position().x();

    for (const auto c : cameras)
        if (x != c->position().x())
        {
            x = qSNaN();
            break;
        }

    if (qIsNaN(x))
        ui->doubleSpinBox->setMultipleValues();
    else
        ui->doubleSpinBox->setValue(x, false);

    // set P.Y
    double y = fst->position().y();

    for (const auto c : cameras)
        if (y != c->position().y())
        {
            y = qSNaN();
            break;
        }

    if (qIsNaN(y))
        ui->doubleSpinBox_2->setMultipleValues();
    else
        ui->doubleSpinBox_2->setValue(y, false);

    // set P.Z
    double z = fst->position().z();

    for (const auto c : cameras)
        if (z != c->position().z())
        {
            z = qSNaN();
            break;
        }

    if (qIsNaN(z))
        ui->doubleSpinBox_3->setMultipleValues();
    else
        ui->doubleSpinBox_3->setValue(z, false);

    // set R.H
    double h = fst->EulerAngles().y();

    for (const auto c : cameras)
        if (h != c->EulerAngles().y())
        {
            h = qSNaN();
            break;
        }

    if (qIsNaN(h))
        ui->doubleSpinBox_4->setMultipleValues();
    else
        ui->doubleSpinBox_4->setValue(h, false);

    // set R.P
    double p = fst->EulerAngles().x();

    for (const auto c : cameras)
        if (p != c->EulerAngles().x())
        {
            p = qSNaN();
            break;
        }

    if (qIsNaN(p))
        ui->doubleSpinBox_5->setMultipleValues();
    else
        ui->doubleSpinBox_5->setValue(p, false);

    // set R.B
    double b = fst->EulerAngles().z();

    for (const auto c : cameras)
        if (b != c->EulerAngles().z())
        {
            b = qSNaN();
            break;
        }

    if (qIsNaN(b))
        ui->doubleSpinBox_6->setMultipleValues();
    else
        ui->doubleSpinBox_6->setValue(b, false);

    // set camera origin
    QVector3D g;

    for (const auto c : cameras)
        g += c->position();

    g /= rows.count();

    Camera::setOrigin(g);
}

void CameraAttributes::updateProjection()
{
    const auto fst = cameras.first();

    // set focal length
    double fl = fst->getFocalLength();

    for (const auto c : cameras)
        if (fl != c->getFocalLength())
        {
            fl = qSNaN();
            break;
        }

    if (qIsNaN(fl))
        ui->doubleSpinBox_7->setMultipleValues();
    else
        ui->doubleSpinBox_7->setValue(fl, false);

    // set sensor size
    double ss = fst->getSensorSize();

    for (const auto c : cameras)
        if (ss != c->getSensorSize())
        {
            ss = qSNaN();
            break;
        }

    if (qIsNaN(ss))
        ui->doubleSpinBox_8->setMultipleValues();
    else
        ui->doubleSpinBox_8->setValue(ss, false);
}
