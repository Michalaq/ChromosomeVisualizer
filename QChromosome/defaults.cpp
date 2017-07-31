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

QMap<int, QPair<const char*, QVariant>> Defaults::tn2defaults = {
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

Defaults::Defaults(QWidget *parent) : QWidget(parent), ui(new Ui::Defaults)
{
    ui->setupUi(this);

    ui->tableView->setItemDelegateForColumn(0, new TableIntDelegate(this));
    ui->tableView->setItemDelegateForColumn(1, new TableNameDelegate(this));
    ui->tableView->setItemDelegateForColumn(2, new TagsDelegate(this));

    auto *m1 = new TableModel({"Binder Type", "Binder Name", "Default Tags"}, this);

    m1->insertRows(0, 2, m1->index(0, 0));
    m1->setData(m1->index(0, 0), 0); m1->setData(m1->index(0, 1), "LAM"); m1->setData(m1->index(0, 2), QVariantList({QVariant::fromValue(new Material("", Qt::blue))}));
    m1->setData(m1->index(1, 0), 1); m1->setData(m1->index(1, 1), "BIN"); m1->setData(m1->index(1, 2), QVariantList({QVariant::fromValue(new Material("", Qt::white, .5))}));

    ui->tableView->setModel(m1);

    /*connect(ui->tableWidget, &QTableWidget::currentItemChanged, [this](QTableWidgetItem* i, QTableWidgetItem*) {
        previous = i->data(Qt::DisplayRole);
        key1 = ui->tableWidget->item(i->row(), 0)->data(Qt::DisplayRole).toInt();
        key = bt2tn[key1];
    });

    connect(ui->tableWidget, &QTableWidget::itemChanged, [this](QTableWidgetItem* i) {
        int c = i->column();
        switch (c)
        {
        case 0: // binder type
            {
                bool ok;
                int v = i->data(Qt::DisplayRole).toInt(&ok);
                if (ok && !bt2tn.contains(v))
                {
                    auto oldtn = bt2tn.take(key1);
                    bt2tn.insert(v, oldtn);
                }
                else
                    i->setData(Qt::DisplayRole, previous);
            }
            break;
        case 1: // binder name
            if (i->data(Qt::DisplayRole).canConvert<QString>())
            {
                dump.append(i->data(Qt::DisplayRole).toByteArray());
                auto v = tn2defaults.take(key);
                v.first = dump.last().constData();
                tn2defaults.insert(key, v);
            }
            else
                i->setData(Qt::DisplayRole, previous);
            break;
        case 2: // binder color
            if (i->data(Qt::DisplayRole).canConvert<QString>())
            {
                auto c = QColor(i->data(Qt::DisplayRole).toString());
                if (c.isValid())
                {
                    auto v = tn2defaults.take(key);
                    v.second = c;
                    tn2defaults.insert(key, v);
                }
                else
                    i->setData(Qt::DisplayRole, previous);
            }
            else
                i->setData(Qt::DisplayRole, previous);
            break;
        }

        previous = i->data(Qt::DisplayRole);
    });*/

    ui->tableView_2->setItemDelegateForColumn(0, new TableVectDelegate(this));
    ui->tableView_2->setItemDelegateForColumn(1, new TableNameDelegate(this));
    ui->tableView_2->setItemDelegateForColumn(2, new TagsDelegate(this));

    auto *m2 = new TableModel({"Energy Vector", "Bead Name", "Default Tags"}, this);

    m2->insertRows(0, 4, m2->index(0, 0));
    m2->setData(m2->index(0, 0), "[0,0]"); m2->setData(m2->index(0, 1), "UNB"); m2->setData(m2->index(0, 2), QVariantList({QVariant::fromValue(new Material("", Qt::red))}));
    m2->setData(m2->index(1, 0), "[0,1]"); m2->setData(m2->index(1, 1), "BOU"); m2->setData(m2->index(1, 2), QVariantList({QVariant::fromValue(new Material("", Qt::green))}));
    m2->setData(m2->index(2, 0), "[1,0]"); m2->setData(m2->index(2, 1), "LAM"); m2->setData(m2->index(2, 2), QVariantList({QVariant::fromValue(new Material("", Qt::blue))}));
    m2->setData(m2->index(3, 0), "[2,0]"); m2->setData(m2->index(3, 1), "LAM"); m2->setData(m2->index(3, 2), QVariantList({QVariant::fromValue(new Material("", Qt::blue))}));

    ui->tableView_2->setModel(m2);

    /*connect(ui->tableWidget_2, &QTableWidget::currentItemChanged, [this](QTableWidgetItem* i, QTableWidgetItem*) {
        previous = i->data(Qt::DisplayRole);
        key2 = parseJsonArray(ui->tableWidget_2->item(i->row(), 0)->data(Qt::DisplayRole).toByteArray());
        key = ev2tn[key2];
    });

    connect(ui->tableWidget_2, &QTableWidget::itemChanged, [this](QTableWidgetItem* i) {
        int c = i->column();
        switch (c)
        {
        case 0: // energy vector
            {
                bool ok;
                auto v = parseJsonArray(i->data(Qt::DisplayRole).toByteArray(), &ok);
                if (ok && !ev2tn.contains(v))
                {
                    auto oldtn = ev2tn.take(key2);
                    ev2tn.insert(v, oldtn);
                }
                else
                    i->setData(Qt::DisplayRole, previous);
            }
            break;
        case 1: // bead name
            if (i->data(Qt::DisplayRole).canConvert<QString>())
            {
                dump.append(i->data(Qt::DisplayRole).toByteArray());
                auto v = tn2defaults.take(key);
                v.first = dump.last().constData();
                tn2defaults.insert(key, v);
            }
            else
                i->setData(Qt::DisplayRole, previous);
            break;
        case 2: // bead color
            if (i->data(Qt::DisplayRole).canConvert<QString>())
            {
                auto c = QColor(i->data(Qt::DisplayRole).toString());
                if (c.isValid())
                {
                    auto v = tn2defaults.take(key);
                    v.second = c;
                    tn2defaults.insert(key, v);
                }
                else
                    i->setData(Qt::DisplayRole, previous);
            }
            else
                i->setData(Qt::DisplayRole, previous);
            break;
        }

        previous = i->data(Qt::DisplayRole);
    });*/

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

                dump.append(v.toUtf8());
                tn2defaults[oldtn].first = dump.last().constData();
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

const char* Defaults::typename2label(int tn)
{
    return tn2defaults[tn].first;
}

QVariant Defaults::typename2color(int tn)
{
    return tn2defaults[tn].second;
}
