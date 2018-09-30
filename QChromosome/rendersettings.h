#ifndef RENDERSETTINGS_H
#define RENDERSETTINGS_H

#include <QMainWindow>

namespace Ui
{
    class RenderSettings;
}

class Session;
class TabWidget;

class RenderSettings : public QMainWindow
{
    Q_OBJECT
public:
    static RenderSettings* getInstance();
    ~RenderSettings();

    void addTabWidget(TabWidget* tabWidget);
    void setSession(Session* session);

private:
    explicit RenderSettings(QWidget *parent = 0);
    static RenderSettings *instance;

    Ui::RenderSettings *ui;
};

#endif // RENDERSETTINGS_H
