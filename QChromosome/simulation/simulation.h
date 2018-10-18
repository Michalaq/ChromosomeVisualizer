#ifndef SIMULATION_H
#define SIMULATION_H


#include "simulationlayer.h"
#include <QVector>

class Session;
class TreeModel;

class SimulationV2 : public QVector<SimulationLayerV2*>
{
public:
    SimulationV2(Session* s);
    ~SimulationV2();

    void loadEntry(int time);

    int cacheHeaders(int time);

    TreeModel* getModel() const;

    void prepend(SimulationLayerV2* value);

private:
    TreeModel* model;
};

#endif // SIMULATION_H
