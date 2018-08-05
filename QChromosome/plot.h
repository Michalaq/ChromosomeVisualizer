#ifndef PLOT_H
#define PLOT_H

#include "softslider.h"

#include "../QtChromosomeViz_v2/bartekm_code/Simulation.h"
#include "legend.h"
#include "multimap.h"

class Session;

class Plot : public SoftSlider
{
    Q_OBJECT
public:
    explicit Plot(Session* s, QWidget *parent = 0);
    ~Plot();

    void updateSimulation();

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

    void paintEvent(QPaintEvent *event);

private:
    Simulation* simulation_;

    QHash<QString, QVector<QPointF> > data;
    Tree minimax;

    int lastBuffered;

    int padding_top = 48;
    int padding_bottom = 33;

    QAbstractSlider *slider;

    QHash<QString, Legend*> legend;

    void addLegend(const QString &fname);

    static const QList<QColor> colorOrder;

    // Samples a list of a monotonically-x-increasing points at given point.
    // It also returns positions of endpoints of the segment from which
    // the returned point was sampled
    static QPointF sampleAtX(qreal x, const QVector<QPointF> &plot,
                             int *lowerIndex = nullptr, int *upperIndex = nullptr);

signals:

public slots:
    void setMaximum(int max);
};

#endif // PLOT_H
