#include "defaults1.h"
#include "ui_defaults1.h"

QMap<int, QVariant> Defaults1::cn2defaults;

#include "tablemodel.h"
#include "tagsdelegate.h"
#include "materialbrowser.h"

Defaults1::Defaults1(QWidget *parent) : QWidget(parent), ui(new Ui::Defaults1)
{
    ui->setupUi(this);

    QVariantList mat[13];

    for (int i = 0; i < 13; i++)
        mat[i] = {QVariant::fromValue(MaterialBrowser::getInstance()->mat[i])};

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

    m4->insertRows(0, 12, m4->index(0, 0));

    for (int i = 0; i < 12; i++)
    {
        m4->setData(m4->index(i, 0), i);
        m4->setData(m4->index(i, 1), mat[i + 1]);
    }
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
