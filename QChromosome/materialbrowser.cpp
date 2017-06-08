#include "materialbrowser.h"
#include "ui_materialbrowser.h"

MaterialBrowser::MaterialBrowser(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MaterialBrowser),
    s(new QSplitter),
    lv(new QListView),
    w(new QWidget)
{
    ui->setupUi(this);

    s->addWidget(w);
    s->addWidget(lv);
    layout()->addWidget(s);
}

MaterialBrowser::~MaterialBrowser()
{
    delete ui;
}
