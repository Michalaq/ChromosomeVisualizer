#include "fileedit.h"

#include <QWidgetAction>
#include <QFileDialog>

FileEdit::FileEdit(QWidget *parent) : LineEdit(parent)
{
    QWidgetAction *widgetAction = new QWidgetAction(this);

    widget = new MediaControl(this);
    widget->setIcon(QIcon(":/lineedit/search"));
    widget->setFocusPolicy(Qt::NoFocus);
    widget->setIconSize(QSize(16, 16));

    widgetAction->setDefaultWidget(widget);

    addAction(widgetAction, QLineEdit::TrailingPosition);

    setType(FE_File);
}

FileEdit::~FileEdit()
{

}

void FileEdit::setType(Type t)
{
    type = t;

    widget->disconnect();

    switch (type)
    {
    case FE_File:
        connect(widget, &MediaControl::clicked, [this]() {
            QString path = QFileDialog::getOpenFileName(0, tr("Open File"), text());

            if (!path.isEmpty())
                setText(path);
        });
        break;
    case FE_Directory:
        connect(widget, &MediaControl::clicked, [this]() {
            QString path = QFileDialog::getExistingDirectory(0, tr("Open Directory"), text(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

            if (!path.isEmpty())
                setText(path);
        });
        break;
    }
}
