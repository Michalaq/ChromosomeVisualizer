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

#include "treeitem.h"

void PickWidget::pick(QPersistentModelIndex object)
{
    if (!object.isValid())
        return;

    switch (object.sibling(object.row(), 1).data().toInt())
    {
    case NodeType::LayerObject:
        icon->setIcon(QIcon(":/objects/layer"));
        break;
    case NodeType::ChainObject:
        icon->setIcon(QIcon(":/objects/chain"));
        break;
    case NodeType::ResidueObject:
        icon->setIcon(QIcon(":/objects/residue"));
        break;
    case NodeType::AtomObject:
        icon->setIcon(QIcon(":/objects/atom"));
        break;
    case NodeType::CameraObject:
        icon->setIcon(QIcon(":/dialogs/film camera"));
        break;
    }

    setText(object.data().toString());

    obj = object;

    update();
}

void PickWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (obj.isValid());
}
