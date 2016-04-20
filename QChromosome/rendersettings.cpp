#include "rendersettings.h"
#include "ui_tabwidget.h"

#include <QProxyStyle>

class MyProxyStyle : public QProxyStyle
{
public:
    QSize sizeFromContents(ContentsType type, const QStyleOption *option, const QSize &size, const QWidget *widget) const
    {
        QSize s(QProxyStyle::sizeFromContents(type, option, size, widget));

        return (type == QStyle::CT_TabBarTab) ? s.transposed() : s;
    }

    void drawControl(ControlElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget = Q_NULLPTR) const
    {
        if (element == QStyle::CE_TabBarTabLabel)
        {
            if (auto tmp = qstyleoption_cast<const QStyleOptionTab*>(option))
            {
                QStyleOptionTab option(*tmp);
                option.shape = QTabBar::RoundedNorth;
                QProxyStyle::drawControl(element, &option, painter, widget);
            }
        }
        else
            QProxyStyle::drawControl(element, option, painter, widget);
    }
};

RenderSettings::RenderSettings(QWidget *parent) :
    QTabWidget(parent),
    ui(new Ui::TabWidget)
{
    ui->setupUi(this);
    tabBar()->setStyle(new MyProxyStyle);
}

RenderSettings::~RenderSettings()
{
    delete ui;
}
