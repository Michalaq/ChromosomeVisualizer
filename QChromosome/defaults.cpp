#include "defaults.h"
#include "ui_defaults.h"

QMap<int, int> Defaults::bt2tn;
QMap<std::vector<int>, int> Defaults::ev2tn;
QMap<std::string, int> Defaults::rs2tn;

QMap<int, QPair<QString, QVariant>> Defaults::tn2defaults = {
    {0, {"(unresolved binder type)", QVariant()}},
    {1, {"(unresolved bead type)", QVariant()}},
    {2, {"(unresolved atom type)", QVariant()}},
};

int Defaults::typenames = 3;

#include "tablemodel.h"
#include "tagsdelegate.h"
#include <QJsonDocument>
#include <QJsonArray>
#include "materialbrowser.h"

Defaults::Defaults(QWidget *parent) : QWidget(parent), ui(new Ui::Defaults)
{
    ui->setupUi(this);

    QVariantList mat[13];

    for (int i = 0; i < 13; i++)
        mat[i] = {QVariant::fromValue(MaterialBrowser::getInstance()->mat[i])};

    // first table
    ui->tableView->setItemDelegateForColumn(0, new TableIntDelegate(this));
    ui->tableView->setItemDelegateForColumn(1, new TableNameDelegate(this));
    ui->tableView->setItemDelegateForColumn(2, new TagsDelegate(this));

    auto *m1 = new TableModel({"Binder Type", "Binder Name", "Default Tags"}, this);

    ui->tableView->setModel(m1);

    connect(m1, &TableModel::foo, [this](QModelIndex ix, QVariant old) {
        auto pk = ix.sibling(ix.row(), 0).data().toInt();
        switch (ix.column())
        {
        case 0: // binder type
            if (old.isValid())
            {
                auto oldtn = bt2tn.take(old.toInt());
                bt2tn.insert(pk, oldtn);
            }
            else
                bt2tn.insert(pk, typenames++);

            break;
        case 1: // binder name
            tn2defaults[bt2tn[pk]].first = ix.data().toString();
            break;
        case 2: // binder color
            tn2defaults[bt2tn[pk]].second = ix.data();
            break;
        }
    });

    m1->insertRows(0, 2, m1->index(0, 0));
    m1->setData(m1->index(0, 0), 0); m1->setData(m1->index(0, 1), "LAM"); m1->setData(m1->index(0, 2), mat[10]);
    m1->setData(m1->index(1, 0), 1); m1->setData(m1->index(1, 1), "BIN"); m1->setData(m1->index(1, 2), mat[0]);

    // second table
    ui->tableView_2->setItemDelegateForColumn(0, new TableVectDelegate(this));
    ui->tableView_2->setItemDelegateForColumn(1, new TableNameDelegate(this));
    ui->tableView_2->setItemDelegateForColumn(2, new TagsDelegate(this));

    auto *m2 = new TableModel({"Energy Vector", "Bead Name", "Default Tags"}, this);

    ui->tableView_2->setModel(m2);

    connect(m2, &TableModel::foo, [this](QModelIndex ix, QVariant old) {
        auto tmp = QJsonDocument::fromJson(ix.sibling(ix.row(), 0).data().toByteArray()).array().toVariantList();
        std::vector<int> pk;
        for (auto i : tmp) pk.push_back(i.toInt());
        switch (ix.column())
        {
        case 0: // energy vector
            if (old.isValid())
            {
                auto tmp = QJsonDocument::fromJson(old.toByteArray()).array().toVariantList();
                std::vector<int> opk;
                for (auto i : tmp) opk.push_back(i.toInt());
                auto oldtn = ev2tn.take(opk);
                ev2tn.insert(pk, oldtn);
            }
            else
                ev2tn.insert(pk, typenames++);

            break;
        case 1: // bead name
            tn2defaults[ev2tn[pk]].first = ix.data().toString();
            break;
        case 2: // bead color
            tn2defaults[ev2tn[pk]].second = ix.data();
            break;
        }
    });

    m2->insertRows(0, 4, m2->index(0, 0));
    m2->setData(m2->index(0, 0), "[0,0]"); m2->setData(m2->index(0, 1), "UNB"); m2->setData(m2->index(0, 2), mat[2]);
    m2->setData(m2->index(1, 0), "[0,1]"); m2->setData(m2->index(1, 1), "BOU"); m2->setData(m2->index(1, 2), mat[6]);
    m2->setData(m2->index(2, 0), "[1,0]"); m2->setData(m2->index(2, 1), "LAM"); m2->setData(m2->index(2, 2), mat[10]);
    m2->setData(m2->index(3, 0), "[2,0]"); m2->setData(m2->index(3, 1), "LAM"); m2->setData(m2->index(3, 2), mat[10]);

    // third table
    ui->tableView_3->setItemDelegateForColumn(0, new TableNameDelegate(this));
    ui->tableView_3->setItemDelegateForColumn(1, new TagsDelegate(this));

    auto *m3 = new TableModel({"Residue Name", "Default Tags"}, this);

    ui->tableView_3->setModel(m3);

    connect(m3, &TableModel::foo, [this](QModelIndex ix, QVariant old) {
        auto pk = ix.sibling(ix.row(), 0).data().toString();
        switch (ix.column())
        {
        case 0: // residue name
            if (old.isValid())
            {
                auto oldtn = rs2tn.take(old.toString().toStdString());
                rs2tn.insert(pk.toStdString(), oldtn);

                tn2defaults[oldtn].first = pk;
            }
            else
            {
                rs2tn.insert(pk.toStdString(), typenames);
                tn2defaults[typenames++].first = pk;
            }

            break;
        case 1: // atom color
            tn2defaults[rs2tn[pk.toStdString()]].second = ix.data();
            break;
        }
    });

    m3->insertRows(0, 4, m3->index(0, 0));
    m3->setData(m3->index(0, 0), "LAM"); m3->setData(m3->index(0, 1), mat[10]);
    m3->setData(m3->index(1, 0), "BIN"); m3->setData(m3->index(1, 1), mat[0]);
    m3->setData(m3->index(2, 0), "UNB"); m3->setData(m3->index(2, 1), mat[2]);
    m3->setData(m3->index(3, 0), "BOU"); m3->setData(m3->index(3, 1), mat[6]);
}

Defaults::~Defaults()
{
    delete ui;
}

int Defaults::bt2typename(int bt)
{
    auto i = bt2tn.find(bt);
    return i != bt2tn.end() ? i.value() : 0;
}

int Defaults::ev2typename(std::vector<int> ev)
{
    auto i = ev2tn.find(ev);
    return i != ev2tn.end() ? i.value() : 1;
}

int Defaults::rs2typename(std::string rs)
{
    auto i = rs2tn.find(rs);
    return i != rs2tn.end() ? i.value() : 2;
}

const QString& Defaults::typename2label(int tn)
{
    return tn2defaults[tn].first;
}

const QVariant& Defaults::typename2color(int tn)
{
    return tn2defaults[tn].second;
}
