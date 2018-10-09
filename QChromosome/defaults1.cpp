#include "defaults1.h"
#include "ui_defaults1.h"

QMap<int, QVariant> Defaults1::cn2defaults;

#include "tablemodel.h"
#include "tagsdelegate.h"
#include "materialbrowser.h"

Defaults1::Defaults1(QWidget *parent) : QWidget(parent), ui(new Ui::Defaults1)
{
    ui->setupUi(this);

    Material** mat = MaterialBrowser::getInstance()->mat;
    auto mat0 = QVariantList({QVariant::fromValue(mat[0])});
    auto mat1 = QVariantList({QVariant::fromValue(mat[1])});
    auto mat2 = QVariantList({QVariant::fromValue(mat[2])});
    auto mat3 = QVariantList({QVariant::fromValue(mat[3])});

    // fourth table
    ui->tableView_4->setItemDelegateForColumn(0, new TableIntDelegate(this));
    ui->tableView_4->setItemDelegateForColumn(1, new TagsDelegate(this));

    auto *m4 = new TableModel({"Chain number", "Default Tags"}, this);

    ui->tableView_4->setModel(m4);

    connect(m4, &TableModel::foo, [this](QModelIndex ix, QVariant old) {
        bool ok;
        int pk;
        switch (ix.column())
        {
        case 0: // chain number
            pk = old.toInt(&ok);

            if (ok)
                cn2defaults.remove(pk);

            cn2defaults.insert(ix.data().toInt(), ix.sibling(ix.row(), 1).data());

            break;
        case 1: // atom color
            pk = ix.sibling(ix.row(), 0).data().toInt(&ok);

            if (ok)
                cn2defaults[pk] = ix.data();

            break;
        }
    });

    m4->insertRows(0, 4, m4->index(0, 0));
    m4->setData(m4->index(0, 0), 0); m4->setData(m4->index(0, 1), mat2);
    m4->setData(m4->index(1, 0), 1); m4->setData(m4->index(1, 1), mat3);
    m4->setData(m4->index(2, 0), 2); m4->setData(m4->index(2, 1), mat0);
    m4->setData(m4->index(3, 0), 3); m4->setData(m4->index(3, 1), mat1);
}

Defaults1::~Defaults1()
{
    delete ui;
}

const QVariant& Defaults1::chainnumber2color(int cn)
{
    static const QVariant tmp;
    auto i = cn2defaults.find(cn);
    return i != cn2defaults.end() ? i.value() : tmp;
}
