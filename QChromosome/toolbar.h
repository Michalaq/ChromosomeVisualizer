#ifndef TOOLBAR_H
#define TOOLBAR_H

#include <QToolBar>

class ToolBar : public QToolBar
{
    Q_OBJECT
public:
    explicit ToolBar(QWidget *parent = 0);

    void addAction(QAction *action);

signals:

public slots:
};

#endif // TOOLBAR_H
