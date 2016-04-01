#ifndef PLOT_H
#define PLOT_H

#include <QWidget>

#include <memory>
#include "../QtChromosomeViz_v2/bartekm_code/Simulation.h"
#include "legend.h"

class Plot : public QWidget
{
    Q_OBJECT
public:
    explicit Plot(QWidget *parent = 0);
    ~Plot();

    void setSimulation(std::shared_ptr<Simulation> dp);
    void setMinimum(int m);
    void setMaximum(int m);
    void setFrame(int n);

protected:
    void paintEvent(QPaintEvent *event);

private:
    std::shared_ptr<Simulation> simulation_;

    QHash<QString, QVector<QPointF> > data;
    qreal maxval;

    int firstFrame;
    int currentFrame;
    int lastFrame;
    int lastBuffered;

    int margin = 15;

    int padding_left = 15;
    int padding_right = 15;
    int padding_top = 40;
    int padding_bottom = 50;

    QHash<QString, Legend*> legend;

signals:

public slots:
};

#endif // PLOT_H
