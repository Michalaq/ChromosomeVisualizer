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

QMap<int, QPair<const char*, QColor>> Defaults::tn2defaults = {
    {0, {"(unresolved binder type)", Qt::gray}},
    {1, {"(unresolved bead type)", Qt::gray}},
    {2, {"LAM", Qt::blue}},
    {3, {"BIN", QColor("#7fffffff")}},
    {4, {"UNB", Qt::red}},
    {5, {"BOU", Qt::green}},
    {6, {"LAM", Qt::blue}},
    {7, {"LAM", Qt::blue}},
    {8, {"(unresolved atom type)", Qt::gray}},
    {9, {"LAM", Qt::blue}},
    {10, {"BIN", QColor("#7fffffff")}},
    {11, {"UNB", Qt::red}},
    {12, {"BOU", Qt::green}},
};

int Defaults::typenames = 8;

#include <QJsonDocument>
#include <QJsonArray>

std::vector<int> parseJsonArray(QByteArray stream, bool *ok = Q_NULLPTR)
{
    auto document = QJsonDocument::fromJson(stream);

    if (!document.isArray())
    {
        *ok = false;
        return {};
    }

    auto array = document.array().toVariantList();

    std::vector<int> ans;

    bool b;

    for (auto x : array)
    {
        auto y = x.toInt(&b);

        if (!b)
        {
            if (ok) *ok = false;
            return {};
        }

        ans.push_back(y);
    }

    if (ok) *ok = true;
    return ans;
}

#include "tablemodel.h"
#include "tagsdelegate.h"

Defaults::Defaults(QWidget *parent) : QWidget(parent), ui(new Ui::Defaults)
{
    ui->setupUi(this);

    ui->tableView->setModel(new TableModel({"Binder Type", "Binder Name", "Default Tags"}, this));

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

    connect(ui->tableWidget_2, &QTableWidget::currentItemChanged, [this](QTableWidgetItem* i, QTableWidgetItem*) {
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
    });

    connect(ui->tableWidget_3, &QTableWidget::currentItemChanged, [this](QTableWidgetItem* i, QTableWidgetItem*) {
        previous = i->data(Qt::DisplayRole);
        key3 = ui->tableWidget_3->item(i->row(), 0)->data(Qt::DisplayRole).toString().toStdString();
        key = rs2tn[key3];
    });

    connect(ui->tableWidget_3, &QTableWidget::itemChanged, [this](QTableWidgetItem* i) {
        int c = i->column();
        switch (c)
        {
        case 0: // residue name
            {
                auto v = i->data(Qt::DisplayRole).toString().toStdString();
                if (!rs2tn.contains(v))
                {
                    auto oldtn = rs2tn.take(key3);
                    rs2tn.insert(v, oldtn);
                }
                else
                    i->setData(Qt::DisplayRole, previous);
            }
            break;
        case 1: // atom color
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
    });

    ui->tableView->setItemDelegateForColumn(2, new TagsDelegate(this));
    ui->tableWidget_2->setItemDelegateForColumn(2, new TagsDelegate(this));
    ui->tableWidget_3->setItemDelegateForColumn(1, new TagsDelegate(this));
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

QColor Defaults::typename2color(int tn)
{
    return tn2defaults[tn].second;
}
