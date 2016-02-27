#include "dockwidget.h"
#include "ui_dockwidget.h"

DockWidget::DockWidget(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::DockWidget)
{
    auto a = new QWidget(this);
    a->setFixedHeight(25);
    ui->setupUi(a);
    setTitleBarWidget(a);
    setMinimumHeight(25);
}

DockWidget::~DockWidget()
{
    delete ui;
}
