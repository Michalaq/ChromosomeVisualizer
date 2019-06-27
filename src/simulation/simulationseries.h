#ifndef SIMULATIONSERIES_H
#define SIMULATIONSERIES_H


#include <QVector>
#include <QLineSeries>
#include "simulationitem.h"

class Session;

class SimulationSeries : public SimulationItem
{
public:
    SimulationSeries(const QString& name, Session* s, int f = 0, int l = INT_MAX, int t = 1);
    virtual ~SimulationSeries();

    static SimulationSeries* read(const QJsonObject& json, Session* session);

protected:
    QVector<QtCharts::QLineSeries*> series;
};

#endif // SIMULATIONSERIES_H
