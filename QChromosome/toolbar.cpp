#include "toolbar.h"
#include <QLayout>

ToolBar::ToolBar(QWidget *parent) : QToolBar(parent)
{
    layout()->setMargin(0);
}

#include <QWidgetAction>
#include "mediacontrol.h"

void ToolBar::addAction(QAction *action)
{
    QWidgetAction *widgetAction = new QWidgetAction(this);

    MediaControl *widget = new MediaControl(this);

    widget->setCheckable(action->isCheckable());
    widget->setChecked(action->isChecked());

    widget->setEnabled(action->isEnabled());

    widget->setIcon(action->icon());
    widget->setIconSize(QSize(24, 24));

    widget->setText(action->text());

    connect(action, &QAction::changed, widget, [widget, action] {
        widget->setStatusTip(action->statusTip());
    });

    if (action->isCheckable())
    {
        connect(action, &QAction::toggled, widget, &MediaControl::setChecked);
        connect(widget, &MediaControl::toggled, action, &QAction::setChecked);
    }
    else
    {
        connect(widget, &MediaControl::clicked, action, &QAction::trigger);
    }

    widget->setFixedSize(minimumSize());

    widgetAction->setDefaultWidget(widget);

    QToolBar::addAction(widgetAction);
}
