#include "dockwidget.h"
#include "ui_dockwidget.h"

DockWidget::DockWidget(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::DockWidget)
{
    auto a = new QWidget(this);
    ui->setupUi(a);
    setTitleBarWidget(a);

    connect(ui->floatButton, &QPushButton::toggled, this, &QDockWidget::setFloating);
    connect(ui->closeButton, &QPushButton::clicked, this, &QDockWidget::close);
    //connect(this, &QDockWidget::topLevelChanged, ui->floatButton, &QPushButton::setChecked);
}

DockWidget::~DockWidget()
{
    delete ui;
}
