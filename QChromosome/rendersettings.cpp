#include "rendersettings.h"
#include "ui_rendersettings.h"

RenderSettings* RenderSettings::instance = nullptr;

#include <QProxyStyle>
#include <QPainter>

class MyProxyStyle : public QProxyStyle
{
public:
    void drawItemText(QPainter *painter, const QRect &rect, int flags, const QPalette &pal, bool enabled, const QString &text, QPalette::ColorRole textRole = QPalette::NoRole) const
    {
        QRect _rect(QPoint(), rect.size().transposed());

        painter->save();

        painter->translate(rect.center());
        painter->rotate(90);
        painter->translate(-_rect.center());

        QProxyStyle::drawItemText(painter, _rect, Qt::AlignLeft | (flags & ~Qt::AlignHorizontal_Mask), pal, enabled, text, textRole);

        painter->restore();
    }
};

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
