#include "dockwidget.h"
#include "ui_dockwidget.h"

QAction* DockWidget::none = new QAction("Empty", 0);

int DockWidget::n = 0;

DockWidget::DockWidget(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::DockWidget)
{
    setTitleBarWidget(new QWidget(this));
    ui->setupUi(titleBarWidget());

    connect(this, &QDockWidget::windowTitleChanged, ui->label, &QLabel::setText);

    connect(ui->floatButton, &QPushButton::clicked, this, &QDockWidget::setFloating);
    connect(ui->closeButton, &QPushButton::clicked, this, &QDockWidget::close);

    n++;

    recclo = new QAction(this);
    recclo->setVisible(!isVisible());

    connect(recclo, &QAction::triggered, [this] {
        setVisible(true);
        recclo->setVisible(false);
    });

    connect(this, &QDockWidget::visibilityChanged, [this](bool f) {
        recclo->setVisible(!f);

        if (!f)
        {
            n++;
            none->setVisible(false);
        }
        else
        {
            if (--n == 0)
                none->setVisible(true);
        }
    });

    connect(this, &QDockWidget::windowTitleChanged, recclo, &QAction::setText);
}

DockWidget::~DockWidget()
{
    delete ui;
}

QAction* DockWidget::recentlyClosedAction()
{
    return recclo;
}

QAction* DockWidget::noneClosedAction()
{
    return none;
}
