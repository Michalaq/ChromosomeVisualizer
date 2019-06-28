#ifndef SIMULATIONLAYER_H
#define SIMULATIONLAYER_H


#include "simulationitem.h"

class Session;

class SimulationLayer : public SimulationItem
{
public:
    SimulationLayer(const QString& name, Session* s, int f = 0, int l = INT_MAX, int t = 1);
    virtual ~SimulationLayer();

    virtual void remove() = 0;
};

#endif // SIMULATIONLAYER_H
