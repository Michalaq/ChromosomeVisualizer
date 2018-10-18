#ifndef SIMULATIONLAYER_H
#define SIMULATIONLAYER_H


class Session;

class SimulationLayerV2
{
public:
    SimulationLayerV2();
    virtual ~SimulationLayerV2();

    virtual void loadEntry(int time, Session* session) = 0;

    virtual int cacheHeaders(int limit) = 0;
};

#endif // SIMULATIONLAYER_H
