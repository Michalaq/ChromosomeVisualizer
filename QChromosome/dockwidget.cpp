#include "dockwidget.h"
#include "ui_dockwidget.h"

DockWidget::DockWidget(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::DockWidget)
{
    setTitleBarWidget(new QWidget(this));
    ui->setupUi(titleBarWidget());

    connect(this, &QDockWidget::windowTitleChanged, ui->label, &QLabel::setText);

    connect(ui->floatButton, &QPushButton::clicked, this, &QDockWidget::setFloating);
    connect(ui->closeButton, &QPushButton::clicked, this, &QDockWidget::close);
}

DockWidget::~DockWidget()
{
    delete ui;
}
