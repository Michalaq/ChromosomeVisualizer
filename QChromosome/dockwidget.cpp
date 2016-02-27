#include "dockwidget.h"
#include "ui_dockwidget.h"
#include <QDebug>
DockWidget::DockWidget(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::DockWidget)
{
    auto a = new QWidget(this);
    ui->setupUi(a);
    setTitleBarWidget(a);

    connect(this, &QDockWidget::windowTitleChanged, ui->label, &QLabel::setText);

    connect(ui->floatButton, &QPushButton::toggled, this, &QDockWidget::setFloating);
    connect(ui->closeButton, &QPushButton::clicked, this, &QDockWidget::close);

    connect(this, &QDockWidget::topLevelChanged, ui->floatButton, [this](bool x) {
        if (x != this->ui->floatButton->isChecked())
        {
        ui->floatButton->disconnect();
        ui->floatButton->toggle();
        connect(ui->floatButton, &QPushButton::toggled, this, &QDockWidget::setFloating);
        }

        //wyślij zdarzenie hoverleave, jeśli trzeba

        if (!x)
        {
            QEvent event(QEvent::HoverLeave);
QApplication::sendEvent(ui->floatButton, &event);
        }
    }
        );
}

DockWidget::~DockWidget()
{
    delete ui;
}
