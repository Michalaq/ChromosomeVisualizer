#include "defaults.h"
#include "ui_defaults.h"

QMap<int, std::string> Defaults::bt2n = {
    {0, "LAM"},
    {1, "BIN"}
}; // maps binder type to its name

QMap<std::vector<int>, std::string> Defaults::ev2n = {
    {{0,0}, "UNB"},
    {{0,1}, "BOU"},
    {{1,0}, "LAM"},
    {{2,0}, "LAM"}
}; // maps energy vector to bead name

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

Defaults::Defaults(QWidget *parent) : QWidget(parent), ui(new Ui::Defaults)
{
    ui->setupUi(this);

    connect(ui->tableWidget, &QTableWidget::currentItemChanged, [this](QTableWidgetItem* i, QTableWidgetItem*) {
        previous = i->data(Qt::DisplayRole);
        key1 = ui->tableWidget->item(i->row(), 0)->data(Qt::DisplayRole).toInt();
    });

    connect(ui->tableWidget, &QTableWidget::itemChanged, [this](QTableWidgetItem* i) {
        int c = i->column();
        switch (c)
        {
        case 0: // binder type
            {
                bool ok;
                int v = i->data(Qt::DisplayRole).toInt(&ok);
                if (ok && !bt2n.contains(v))
                {
                    auto s = bt2n.take(key1);
                    bt2n.insert(v, s);
                }
                else
                    i->setData(Qt::DisplayRole, previous);
            }
            break;
        case 1: // binder name
            if (i->data(Qt::DisplayRole).canConvert<QString>())
                bt2n.insert(key1, i->data(Qt::DisplayRole).toString().toStdString());
            else
                i->setData(Qt::DisplayRole, previous);
            break;
        case 2: // binder color
            break;
        }

        previous = i->data(Qt::DisplayRole);
    });

    connect(ui->tableWidget_2, &QTableWidget::currentItemChanged, [this](QTableWidgetItem* i, QTableWidgetItem*) {
        previous = i->data(Qt::DisplayRole);
        key2 = parseJsonArray(ui->tableWidget_2->item(i->row(), 0)->data(Qt::DisplayRole).toByteArray());
    });
}

Defaults::~Defaults()
{
    delete ui;
}

std::string Defaults::bt2name(int bt)
{
    auto i = bt2n.find(bt);
    return i != bt2n.end() ? i.value() : "(undefined binder type)";
}

std::string Defaults::ev2name(std::vector<int> ev)
{
    auto i = ev2n.find(ev);
    return i != ev2n.end() ? i.value() : "(undefined bead type)";
}
