#ifndef PLOT_H
#define PLOT_H

#include <QWidget>

#include <memory>
#include "../QtChromosomeViz_v2/bartekm_code/Simulation.h"

class Plot : public QWidget
{
    Q_OBJECT
public:
    explicit Plot(QWidget *parent = 0);
    ~Plot();

    void setSimulation(std::shared_ptr<Simulation> dp);
    void setMaximum(int m);

protected:
    void paintEvent(QPaintEvent *event);

private:
    std::shared_ptr<Simulation> simulation_;

    QPainterPath data;
    QSize size;

    int lastFrame;

signals:

public slots:
};

#endif // PLOT_H
