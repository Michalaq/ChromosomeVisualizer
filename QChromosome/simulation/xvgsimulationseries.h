#ifndef XVGSIMULATIONSERIES_H
#define XVGSIMULATIONSERIES_H


#include "simulationseries.h"

class XVGSimulationSeries : public SimulationSeries
{
public:
    XVGSimulationSeries(const QString& name, Session* s, int f = 0, int l = INT_MAX, int t = 1, bool b = true);
    ~XVGSimulationSeries() override;

    int cacheHeaders(int time) override;

    int lastEntry() const override;

private:
    QByteArray buffer;

    int i = 0, j = 0;
    int line = 0;

    bool atEnd = false;

    void skipHeader();

    QMap<int, QtCharts::QLineSeries*> legend;
};

#endif // XVGSIMULATIONSERIES_H
