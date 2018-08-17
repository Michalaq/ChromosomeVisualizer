#ifndef RENDERSETTINGS_H
#define RENDERSETTINGS_H

#include <QMainWindow>

namespace Ui
{
    class RenderSettings;
}

class RenderSettings : public QMainWindow
{
    Q_OBJECT
public:
    static RenderSettings* getInstance();
    ~RenderSettings();

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

    /* Ambient */
    QString ambient() const;

    /* Diffuse */
    QString diffuse() const;

    /* Phong */
    QString phong() const;

    /* Phong Size */
    QString phongSize() const;

    /* Metallic */
    QString metallic() const;

    /* Iridescence */
    QString iridescence() const;

    /* Iridescence Thickness */
    QString iridescenceThickness() const;

    /* Iridescence Turbulence */
    QString iridescenceTurbulence() const;

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

    qreal aspectRatio;
    QSize resolution;

    qreal widthUnit;
    qreal resolutionUnit;

    //TODO

    QString currentUnit;
    QString currentResolutionUnit;

    QSize outSize;

    /* metric unit conversion table */
    QHash<QString, qreal> units;

    void updateOutputSize();
    //TODO

signals:
    void aspectRatioChanged(qreal);

public slots:

};

#endif // RENDERSETTINGS_H
