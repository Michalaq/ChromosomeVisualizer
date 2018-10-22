#ifndef PLOT_H
#define PLOT_H

#include "softslider.h"
#include "legend.h"
#include <QAbstractSeries>

class Session;

class Plot : public SoftSlider
{
    Q_OBJECT
public:
    explicit Plot(Session* s, QWidget *parent = 0);
    ~Plot();

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

    void paintEvent(QPaintEvent *event);

private:
    Session* session;

    int padding_top = 48;
    int padding_bottom = 33;

    QMap<QtCharts::QAbstractSeries*, Legend*> legend;

    void addLegend(QtCharts::QAbstractSeries* series);

    static const QList<QColor> colorOrder;
};

#endif // PLOT_H
