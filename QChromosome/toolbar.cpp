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
    widget->setIconSize(iconSize());

    widget->setText(action->text());

    connect(action, &QAction::toggled, widget, &MediaControl::setChecked);
    connect(widget, &MediaControl::toggled, action, &QAction::setChecked);

    widget->setFixedSize(50, 50);

    widgetAction->setDefaultWidget(widget);

    QToolBar::addAction(widgetAction);
}
