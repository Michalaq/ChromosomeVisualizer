#ifndef SIMULATIONLAYER_H
#define SIMULATIONLAYER_H


#include "simulationitem.h"

class TreeItem;
class Session;

class SimulationLayer : public SimulationItem
{
public:
    SimulationLayer(const QString& name, Session* s, int f = 0, int l = INT_MAX, int t = 1);
    virtual ~SimulationLayer();

    virtual TreeItem* getModel() const;
    static SimulationLayer* read(const QJsonObject& json, Session* session);

    virtual void remove() = 0;

protected:
    TreeItem* model;
};

#endif // SIMULATIONLAYER_H
