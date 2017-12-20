#include "attributes.h"
#include "ui_attributes.h"

Attributes::Attributes(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Attributes)
{
    ui->setupUi(this);
}

Attributes::~Attributes()
{
    delete ui;
}

#include "treeitem.h"
#include "camera.h"

void Attributes::setSelection(QModelIndexList &selectedRows, QModelIndexList &selectedAtoms)
{
    if (selectedRows.isEmpty())
        return hide();

    rows.swap(selectedRows);
    atoms.swap(selectedAtoms);

    // set title
    QString title = QString::number(rows.count()) + " elements ";

    QString list;

    for (const auto& r : rows)
        list += r.data().toString() + ", ";

    list.chop(2);

    ui->label->setContents(title, list);

    // set name

    // set coordinates and camera origin
    QVector3D g;

    for (const auto& a : atoms)
    {
        const float* pos = reinterpret_cast<AtomItem*>(a.internalPointer())->getInstance().position;
        g += { pos[0], pos[1], pos[2] };
    }

    g /= atoms.count();

    Camera::setOrigin(g);

    show();
}
