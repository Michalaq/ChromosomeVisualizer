#ifndef TABWIDGET_H
#define TABWIDGET_H

#include <QTabWidget>

namespace Ui {
class TabWidget;
}

class Session;

class TabWidget : public QTabWidget
{
    Q_OBJECT

public:
    explicit TabWidget(Session* s, QWidget *parent = nullptr);
    ~TabWidget();

protected:
    void connectNotify(const QMetaMethod& signal);

private:
    Ui::TabWidget *ui;
    Session* session;

    qreal resolution;
    QSize imageResolution;

    qreal widthUnit;
    qreal resolutionUnit;

signals:
    void filmRatioChanged(double);
};

#endif // TABWIDGET_H
