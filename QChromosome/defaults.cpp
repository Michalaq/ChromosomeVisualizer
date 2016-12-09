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

Defaults::Defaults(QWidget *parent) : QWidget(parent), ui(new Ui::Defaults)
{
    ui->setupUi(this);

    connect(ui->tableWidget, &QTableWidget::currentItemChanged, [this](QTableWidgetItem* i, QTableWidgetItem*) {
        previous = i->data(Qt::DisplayRole);
        key = ui->tableWidget->item(i->row(), 0)->data(Qt::DisplayRole).toInt();
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
                    auto s = bt2n.take(key);
                    bt2n.insert(v, s);
                }
                else
                    i->setData(Qt::DisplayRole, previous);
            }
            break;
        case 1: // binder name
            if (i->data(Qt::DisplayRole).canConvert<QString>())
                bt2n.insert(key, i->data(Qt::DisplayRole).toString().toStdString());
            else
                i->setData(Qt::DisplayRole, previous);
            break;
        case 2: // binder color
            break;
        }

        previous = i->data(Qt::DisplayRole);
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
