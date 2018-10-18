#ifndef SIMULATIONLAYER_H
#define SIMULATIONLAYER_H


class TreeItem;

class SimulationLayerV2
{
public:
    virtual ~SimulationLayerV2();

    virtual void loadEntry(int time) = 0;

    virtual int cacheHeaders(int time) = 0;

    virtual TreeItem* getModel() const = 0;
};

#endif // SIMULATIONLAYER_H
