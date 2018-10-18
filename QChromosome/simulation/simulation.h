#ifndef SIMULATION_H
#define SIMULATION_H


#include "simulationlayer.h"
#include <QVector>

class Session;

class SimulationV2 : public QVector<SimulationLayerV2*>
{
public:
    SimulationV2(Session* s);
    ~SimulationV2();

    void loadEntry(int time);

    int cacheHeaders(int limit);

private:
    Session* session;
};

#endif // SIMULATION_H
