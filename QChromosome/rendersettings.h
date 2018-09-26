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

    //TODO
    /* Save */
    QString saveFile() const;

    /* Timestamp */
    bool timestamp() const;

    /* Open File After Rendering  */
    bool openFile() const;

    /* Framerate */
    int framerate() const;

    bool render() const;
    bool exportPOV() const;
    QString POVfileName() const;
    //TODO

private:
    explicit RenderSettings(QWidget *parent = 0);
    static RenderSettings *instance;

    Ui::RenderSettings *ui;
};

#endif // RENDERSETTINGS_H
