#include "defaults.h"
#include "ui_defaults.h"

QMap<int, std::string> Defaults::bt2n = {{0, "LAM"},{1,"BIN"}}; // maps binder type to its name
QMap<std::vector<int>, std::string> Defaults::ev2n = {{{0,0}, "UNB"}, {{0,1}, "BOU"}, {{1,0}, "LAM"}, {{2,0}, "LAM"}}; // maps energy vector to bead name

Defaults::Defaults(QWidget *parent) : QWidget(parent), ui(new Ui::Defaults)
{
    ui->setupUi(this);
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
