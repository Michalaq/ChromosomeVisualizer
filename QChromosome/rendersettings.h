#ifndef RENDERSETTINGS_H
#define RENDERSETTINGS_H

#include <QTabWidget>

namespace Ui
{
    class TabWidget;
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

protected:
    void connectNotify(const QMetaMethod& signal);

private:
    Ui::TabWidget *ui;

    qreal aspectRatio;
    QString currentUnit;
    QString currentResolutionUnit;

    /* metric unit conversion table */
    QHash<QString, qreal> units;

signals:
    void aspectRatioChanged(qreal ar);

public slots:

};

#endif // RENDERSETTINGS_H
