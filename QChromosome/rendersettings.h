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
    /* Output */
    QSize outputSize() const; //px

    /* Save */
    QString saveFile() const;

    /* Quality */
    QString quality() const;

    /* Anti-Aliasing */
    bool antiAliasing() const;

    /* AA Sampling Method */
    QString aaSamplingMethod() const;

    /* AA Threshold */
    QString aaThreshold() const;

    /* AA Jitter */
    bool aaJitter() const;

    /* AA Jitter Amount */
    QString aaJitterAmount() const;

    /* AA Depth */
    QString aaDepth() const;

    /* Timestamp */
    bool timestamp() const;

    /* 360 Degrees Camera  */
    bool cam360() const;

    /* Open File After Rendering  */
    bool openFile() const;

    /* Framerate */
    int framerate() const;

    /* Paint overlays */
    bool overlays() const;

    bool render() const;
    bool exportPOV() const;
    QString POVfileName() const;
    //TODO

protected:
    void connectNotify(const QMetaMethod& signal);

private:
    explicit RenderSettings(QWidget *parent = 0);
    static RenderSettings *instance;

    Ui::RenderSettings *ui;

    //TODO
    qreal aspectRatio;
    QString currentUnit;
    QString currentResolutionUnit;

    QSize outSize;

    /* metric unit conversion table */
    QHash<QString, qreal> units;

    void updateOutputSize();
    //TODO

signals:
    void aspectRatioChanged(double);

public slots:

};

#endif // RENDERSETTINGS_H
