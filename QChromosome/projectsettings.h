#ifndef PROJECTSETTINGS_H
#define PROJECTSETTINGS_H

#include <QWidget>

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

private:
    Ui::Form *ui;

signals:

public slots:

friend class MainWindow;
};

#endif // PROJECTSETTINGS_H