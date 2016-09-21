#include "fileedit.h"

#include <QWidgetAction>
#include "mediacontrol.h"
#include <QFileDialog>

FileEdit::FileEdit(QWidget *parent) : LineEdit(parent)
{
    QWidgetAction *widgetAction = new QWidgetAction(this);

    MediaControl *widget = new MediaControl(this);
    widget->setIcon(QIcon(":/lineedit/search"));
    widget->setCursor(Qt::PointingHandCursor);
    widget->setFocusPolicy(Qt::NoFocus);

    widgetAction->setDefaultWidget(widget);

    addAction(widgetAction, QLineEdit::TrailingPosition);

    connect(widget, &MediaControl::clicked, [this]() {
        QString path = QFileDialog::getExistingDirectory(this, tr("Open Directory"), text(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

        if (!path.isEmpty())
            setText(path);
    });
}

FileEdit::~FileEdit()
{

}
