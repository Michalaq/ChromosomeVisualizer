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
    rows.swap(selectedRows);
    atoms.swap(selectedAtoms);

    QVector3D g;

    for (const auto& a : atoms)
    {
        const float* pos = reinterpret_cast<AtomItem*>(a.internalPointer())->getInstance().position;
        g += { pos[0], pos[1], pos[2] };
    }

    g /= atoms.size();

    Camera::setOrigin(g);
}
