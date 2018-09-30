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
    case FE_SaveFileName:
        connect(widget, &MediaControl::clicked, [this]() {
            QString path = QFileDialog::getSaveFileName(0, tr("Save File"), text());

            if (!path.isEmpty())
                setText(path);
        });
        break;
    case FE_OpenFileName:
        connect(widget, &MediaControl::clicked, [this]() {
            QString path = QFileDialog::getOpenFileName(0, tr("Open File"), text());

            if (!path.isEmpty())
                setText(path);
        });
        break;
    case FE_ExistingDirectory:
        connect(widget, &MediaControl::clicked, [this]() {
            QString path = QFileDialog::getExistingDirectory(0, tr("Open Directory"), text(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

            if (!path.isEmpty())
                setText(path);
        });
        break;
    }
}

void FileEdit::setReadOnly(bool b)
{
    LineEdit::setReadOnly(b);
    widget->setEnabled(!b);
}
