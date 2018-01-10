#include "pickwidget.h"
#include "mediacontrol.h"

QSignalMapper PickWidget::sm;

PickWidget::PickWidget(QWidget *parent) :
    LineEdit(parent),
    icon(new QAction(this))
{
    addAction(icon, QLineEdit::LeadingPosition);

    QWidgetAction *widgetAction = new QWidgetAction(this);

    auto widget = new MediaControl(this);
    widget->setIcon(QIcon(":/lineedit/pick"));
    widget->setFocusPolicy(Qt::NoFocus);
    widget->setIconSize(QSize(16, 16));

    widgetAction->setDefaultWidget(widget);

    addAction(widgetAction, QLineEdit::TrailingPosition);

    connect(widget, &MediaControl::clicked, [this]() {
        sm.map(this);
    });

    sm.setMapping(this, this);
}

const QSignalMapper& PickWidget::getSignalMapper()
{
    return sm;
}

void PickWidget::pick(const QPersistentModelIndex &object, bool spontaneous)
{
    if (!object.isValid() && spontaneous)
        return;

    icon->setIcon(object.data(Qt::DecorationRole).value<QIcon>());

    setText(object.data().toString());

    obj = object;

    if (spontaneous)
        emit picked(obj);
}

void PickWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (obj.isValid())
        emit selected(obj);

    QLineEdit::mouseDoubleClickEvent(event);
}
