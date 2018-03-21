#ifndef PROJECTSETTINGS_H
#define PROJECTSETTINGS_H

#include <QWidget>
#include "session.h"

namespace Ui
{
    class Form;
}

class ProjectSettings : public QWidget
{
    Q_OBJECT
public:
    explicit ProjectSettings(QWidget *parent = 0);
    ~ProjectSettings();

    void setSession(Session* s);

private:
    Ui::Form *ui;
    Session* session;

signals:

public slots:

    friend class Session;
};

#endif // PROJECTSETTINGS_H
