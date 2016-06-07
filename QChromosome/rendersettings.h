#ifndef RENDERSETTINGS_H
#define RENDERSETTINGS_H

#include <QTabWidget>

namespace Ui
{
    class TabWidget;
    class RenderSettingsWidget;
}

class RenderSettings : public QTabWidget
{
    Q_OBJECT
public:
    explicit RenderSettings(QWidget *parent = 0);
    ~RenderSettings();

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

protected:
    void connectNotify(const QMetaMethod& signal);

private:
    Ui::TabWidget *ui;

    qreal aspectRatio;
    QString currentUnit;
    QString currentResolutionUnit;

    QSize outSize;

    /* metric unit conversion table */
    QHash<QString, qreal> units;

    void updateOutputSize();

signals:
    void aspectRatioChanged(qreal ar);

public slots:

};

class RenderSettingsWidget : public QWidget
{
    Q_OBJECT
public:
    explicit RenderSettingsWidget(RenderSettings *rs, QWidget *parent = 0);
    ~RenderSettingsWidget();

private:
    Ui::RenderSettingsWidget *ui;
};

#endif // RENDERSETTINGS_H
