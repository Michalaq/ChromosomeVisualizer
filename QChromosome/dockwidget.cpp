#include "dockwidget.h"
#include "ui_dockwidget.h"
#include <QDebug>
DockWidget::DockWidget(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::DockWidget)
{
    setTitleBarWidget(new QWidget(this));
    ui->setupUi(titleBarWidget());

    connect(this, &QDockWidget::windowTitleChanged, ui->label, &QLabel::setText);

    connect(ui->floatButton, &QPushButton::clicked, this, &QDockWidget::setFloating);
    connect(ui->closeButton, &QPushButton::clicked, this, &QDockWidget::close);

    connect(this, &QDockWidget::topLevelChanged, ui->floatButton,
            [this](bool topLevel)
    {
        if (topLevel != ui->floatButton->isChecked())
            ui->floatButton->toggle();

        if (!topLevel)
        {
            QEvent event(QEvent::HoverLeave);
            QApplication::sendEvent(ui->floatButton, &event);
        }
    });
}

DockWidget::~DockWidget()
{
    delete ui;
}
