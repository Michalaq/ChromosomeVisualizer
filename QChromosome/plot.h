#ifndef PLOT_H
#define PLOT_H

#include "softslider.h"

#include <memory>
#include "../QtChromosomeViz_v2/bartekm_code/Simulation.h"
#include "../QtChromosomeViz_v2/bartekm_code/NullSimulationLayer.h"
#include "legend.h"

class Plot : public SoftSlider
{
    Q_OBJECT
public:
    explicit Plot(QWidget *parent = 0);
    ~Plot();

    void setSimulation(std::shared_ptr<Simulation> dp);

    void setMaximum(int max);

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

    void paintEvent(QPaintEvent *event);

private:
    std::shared_ptr<Simulation> simulation_;

    QHash<QString, QVector<QPointF> > data;
    qreal maxval;

    int lastBuffered;

    int label;
    int padding_left = 15;
    int padding_right = 15;
    int padding_top = 20;
    int padding_bottom = 60;

    QHash<QString, Legend*> legend;

    static const QList<QColor> colorOrder;

signals:

public slots:
};

#endif // PLOT_H
