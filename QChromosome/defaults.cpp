#include "defaults.h"
#include "ui_defaults.h"

QMap<int, int> Defaults::bt2tn = {
    {0, 2},
    {1, 3}
}; // maps binder type to its name

QMap<std::vector<int>, int> Defaults::ev2tn = {
    {{0,0}, 4},
    {{0,1}, 5},
    {{1,0}, 6},
    {{2,0}, 7}
}; // maps energy vector to bead name

QMap<std::string, int> Defaults::rs2tn = {
    {"LAM", 9},
    {"BIN", 10},
    {"UNB", 11},
    {"BOU", 12}
}; // maps residue name to atom name

QMap<int, QPair<QString, QVariant>> Defaults::tn2defaults = {
    {0, {"(unresolved binder type)", QVariant()}},
    {1, {"(unresolved bead type)", QVariant()}},
    {2, {"LAM", QVariant()}},
    {3, {"BIN", QVariant()}},
    {4, {"UNB", QVariant()}},
    {5, {"BOU", QVariant()}},
    {6, {"LAM", QVariant()}},
    {7, {"LAM", QVariant()}},
    {8, {"(unresolved atom type)", QVariant()}},
    {9, {"LAM", QVariant()}},
    {10, {"BIN", QVariant()}},
    {11, {"UNB", QVariant()}},
    {12, {"BOU", QVariant()}},
};

int Defaults::typenames = 8;

#include "tablemodel.h"
#include "tagsdelegate.h"
#include <QJsonDocument>
#include <QJsonArray>

Defaults::Defaults(QWidget *parent) : QWidget(parent), ui(new Ui::Defaults)
{
    ui->setupUi(this);

    // first table
    ui->tableView->setItemDelegateForColumn(0, new TableIntDelegate(this));
    ui->tableView->setItemDelegateForColumn(1, new TableNameDelegate(this));
    ui->tableView->setItemDelegateForColumn(2, new TagsDelegate(this));

    auto *m1 = new TableModel({"Binder Type", "Binder Name", "Default Tags"}, this);

    ui->tableView->setModel(m1);

    connect(m1, &TableModel::foo, [this](QModelIndex ix, QVariant old) {
        switch (ix.column())
        {
        case 0: // binder type
            if (old.isValid())
            {
                auto oldtn = bt2tn.take(old.toInt());
                bt2tn.insert(ix.data().toInt(), oldtn);
            }

            break;
        case 1: // binder name
            tn2defaults[bt2tn[ix.sibling(ix.row(), 0).data().toInt()]].first = ix.data().toString();
            break;
        case 2: // binder color
            tn2defaults[bt2tn[ix.sibling(ix.row(), 0).data().toInt()]].second = ix.data();
            break;
        }
    });

    m1->insertRows(0, 2, m1->index(0, 0));
    m1->setData(m1->index(0, 0), 0); m1->setData(m1->index(0, 1), "LAM"); m1->setData(m1->index(0, 2), QVariantList({QVariant::fromValue(new Material("", Qt::blue))}));
    m1->setData(m1->index(1, 0), 1); m1->setData(m1->index(1, 1), "BIN"); m1->setData(m1->index(1, 2), QVariantList({QVariant::fromValue(new Material("", Qt::white, .5))}));

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
    m2->setData(m2->index(0, 0), "[0,0]"); m2->setData(m2->index(0, 1), "UNB"); m2->setData(m2->index(0, 2), QVariantList({QVariant::fromValue(new Material("", Qt::red))}));
    m2->setData(m2->index(1, 0), "[0,1]"); m2->setData(m2->index(1, 1), "BOU"); m2->setData(m2->index(1, 2), QVariantList({QVariant::fromValue(new Material("", Qt::green))}));
    m2->setData(m2->index(2, 0), "[1,0]"); m2->setData(m2->index(2, 1), "LAM"); m2->setData(m2->index(2, 2), QVariantList({QVariant::fromValue(new Material("", Qt::blue))}));
    m2->setData(m2->index(3, 0), "[2,0]"); m2->setData(m2->index(3, 1), "LAM"); m2->setData(m2->index(3, 2), QVariantList({QVariant::fromValue(new Material("", Qt::blue))}));

    // third table
    ui->tableView_3->setItemDelegateForColumn(0, new TableNameDelegate(this));
    ui->tableView_3->setItemDelegateForColumn(1, new TagsDelegate(this));

    auto *m3 = new TableModel({"Residue Name", "Default Tags"}, this);

    ui->tableView_3->setModel(m3);

    connect(m3, &TableModel::foo, [this](QModelIndex ix, QVariant old) {
        switch (ix.column())
        {
        case 0: // residue name
            if (old.isValid())
            {
                auto v = ix.data().toString();

                auto oldtn = rs2tn.take(old.toString().toStdString());
                rs2tn.insert(v.toStdString(), oldtn);

                tn2defaults[oldtn].first = v;
            }

            break;
        case 1: // atom color
            tn2defaults[rs2tn[ix.sibling(ix.row(), 0).data().toString().toStdString()]].second = ix.data();
            break;
        }
    });

    m3->insertRows(0, 4, m3->index(0, 0));
    m3->setData(m3->index(0, 0), "LAM"); m3->setData(m3->index(0, 1), QVariantList({QVariant::fromValue(new Material("", Qt::blue))}));
    m3->setData(m3->index(1, 0), "BIN"); m3->setData(m3->index(1, 1), QVariantList({QVariant::fromValue(new Material("", Qt::white, .5))}));
    m3->setData(m3->index(2, 0), "UNB"); m3->setData(m3->index(2, 1), QVariantList({QVariant::fromValue(new Material("", Qt::red))}));
    m3->setData(m3->index(3, 0), "BOU"); m3->setData(m3->index(3, 1), QVariantList({QVariant::fromValue(new Material("", Qt::green))}));
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
    return i != rs2tn.end() ? i.value() : 8;
}

const QString& Defaults::typename2label(int tn)
{
    return tn2defaults[tn].first;
}

const QVariant& Defaults::typename2color(int tn)
{
    return tn2defaults[tn].second;
}
