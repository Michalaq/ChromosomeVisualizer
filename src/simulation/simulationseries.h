#ifndef SIMULATIONSERIES_H
#define SIMULATIONSERIES_H


#include <QVector>
#include <QLineSeries>
#include "iodevice.h"

class Session;

class SimulationSeries
{
public:
    SimulationSeries(const QString& name, Session* s, int f = 0, int l = INT_MAX, int t = 1);
    virtual ~SimulationSeries();

    virtual int cacheHeaders(int time) = 0;

    virtual int lastEntry() const = 0;

protected:
    Session* session;
    int first, last, stride;

    IODevice* file;

    QVector<QtCharts::QLineSeries*> series;
};

#endif // SIMULATIONSERIES_H
