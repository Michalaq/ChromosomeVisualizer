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
    double getOutputWidth() const;
    double getOutputHeight() const;

private:
    Ui::TabWidget *ui;

signals:

public slots:
};

#endif // RENDERSETTINGS_H
