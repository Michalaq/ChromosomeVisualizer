#include "rendersettings.h"
#include "ui_rendersettings.h"

RenderSettings* RenderSettings::instance = nullptr;

RenderSettings::RenderSettings(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::RenderSettings)
{
    ui->setupUi(this);
    setWindowFlags(Qt::WindowStaysOnTopHint);
}

RenderSettings::~RenderSettings()
{
    delete ui;
}

RenderSettings* RenderSettings::getInstance()
{
    return instance ? instance : instance = new RenderSettings();
}

#include "tabwidget.h"
#include "myproxystyle.h"

void RenderSettings::addTabWidget(TabWidget* tabWidget)
{
    ui->stackedWidget->addWidget(tabWidget);
    tabWidget->tabBar()->setStyle(new MyProxyStyle);
}

#include "session.h"

void RenderSettings::setSession(Session* session)
{
    ui->stackedWidget->setCurrentWidget(session->renderSettings);
}
