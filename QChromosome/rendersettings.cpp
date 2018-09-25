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

    ui->TabWidget->tabBar()->setStyle(new MyProxyStyle);
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
}

#include "session.h"

void RenderSettings::setSession(Session* session)
{
    ui->stackedWidget->setCurrentWidget(session->renderSettings);
}

//TODO
QString RenderSettings::saveFile() const
{
    return ui->lineEdit->text();
}

bool RenderSettings::timestamp() const
{
    return ui->timestampCheckBox->isChecked();
}

bool RenderSettings::openFile() const
{
    return ui->openFileCheckBox->isChecked();
}

int RenderSettings::framerate() const
{
    return ui->spinBox->value();
}

bool RenderSettings::overlays() const
{
    return ui->checkBox_4->isChecked();
}

bool RenderSettings::render() const
{
    return ui->checkBox_2->isChecked();
}

bool RenderSettings::exportPOV() const
{
    return ui->checkBox_3->isChecked();
}

QString RenderSettings::POVfileName() const
{
    return ui->lineEdit_3->text();
}
//TODO
